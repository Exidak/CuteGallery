#include "imagelistview.h"

#include <QImage>
#include <QPaintEvent>
#include <QScrollBar>
#include <QStylePainter>
#include <QThread>
#include <QTimer>
#include <QtConcurrent>
#include <QtDebug>
#include "defines.h"

ImageListView::ImageListView(QWidget* parent)
    : QAbstractItemView(parent)
    //, m_columnCount{ 5 }
    , m_thumb_size(MIN_THUMBNAIL_SIZE)
    , m_loadingDelayTimer{ new QTimer{ this } }
    , m_updatingDelayTimer{ new QTimer{ this } }
    , m_imageCache(1)
{
    horizontalScrollBar()->setRange(0, 0);
    verticalScrollBar()->setRange(0, 0);
    setSelectionMode(ExtendedSelection);
    setSelectionBehavior(SelectItems);

    //  подписываемся на таймер отложенной загрузки
    m_loadingDelayTimer->setSingleShot(true);
    connect(m_loadingDelayTimer, &QTimer::timeout, [this] {
        qDebug() << "Scroll Delay Timer Fired";
        startAsyncImageLoading();
    });
    connect(&m_imageLoadingFutureWatcher,
        &QFutureWatcherBase::progressRangeChanged, [](int min, int max) {
            qDebug() << "progressRangeChanged(" << min << ", " << max << ")";
        });
    connect(&m_imageLoadingFutureWatcher,
        &QFutureWatcherBase::progressValueChanged, [](int val) {
            qDebug() << "progressValueChanged(" << val << ")";
        });
    connect(&m_imageLoadingFutureWatcher,
        &QFutureWatcherBase::started, []() {
            qDebug() << "started()";
        });
    connect(&m_imageLoadingFutureWatcher,
        &QFutureWatcherBase::finished, []() {
            qDebug() << "finished()";
        });
    connect(&m_imageLoadingFutureWatcher,
        &QFutureWatcherBase::resultReadyAt,
        [](int index) {
            qDebug() << "resultReadyAt(" << index << ")";
        });

    connect(&m_imageLoadingFutureWatcher,
        &QFutureWatcherBase::resultsReadyAt,
        [this](int begin, int end) {
            for (int index = begin; index < end; ++index) {
                auto item = m_imageLoadingFutureWatcher.resultAt(index);
                m_invalidatingModelRows.append(item->row);
                m_imageCache.insert(item->imageFileName, item->image.release());
            }
            if (!m_updatingDelayTimer->isActive())
                m_updatingDelayTimer->start(250);
        });

    //
    m_updatingDelayTimer->setSingleShot(true);
    connect(m_updatingDelayTimer, &QTimer::timeout, [this] {
        QRect invalidatingRect;
        for (auto&& row : m_invalidatingModelRows) {
            auto rect = visualRect(model()->index(row, 0, rootIndex()));
            invalidatingRect = invalidatingRect.united(rect);
        }
        if (viewport()->rect().intersects(invalidatingRect)) {
            viewport()->update(invalidatingRect);
        }
    });
}

void ImageListView::startScrollDelayTimer()
{
    stopScrollDelayTimer();
    m_loadingDelayTimer->start(200);
}

void ImageListView::stopScrollDelayTimer()
{
    m_updatingDelayTimer->stop();
    stopAsyncImageLoading();
    m_loadingDelayTimer->stop();
}

void ImageListView::setThumbnailSize(int size)
{
    m_thumb_size = size;
    setColumnCount(width() / m_thumb_size);
}

int ImageListView::columnCount() const
{
    return width() / m_thumb_size;
}

void ImageListView::setColumnCount(int columnCount)
{
    reset();
}

QPair<int, int> ImageListView::modelRowRangeForViewportRect(const QRect& rect)
{
    QRect r = rect.normalized();
    int rowCount = model()->rowCount(rootIndex());
    int begin = 0;
    {
        QModelIndex startIndex = indexAt(r.topLeft());
        if (startIndex.isValid()) {
            begin = startIndex.row();
        }
    }
    int end = begin;
    {
        QModelIndex finishIndex = indexAt(r.bottomRight());
        if (finishIndex.isValid()) {
            end = finishIndex.row() + 1;
        } else {
            end = rowCount;
        }
    }
    return QPair<int, int>(begin, end);
}

void ImageListView::startAsyncImageLoading()
{
    class ImageLoader {
    public:
        typedef ImageLoadingTaskSharedPtr result_type;

    public:
        ImageLoadingTaskSharedPtr operator()(ImageLoadingTaskSharedPtr task)
        {
            if (!task->image) {
                task->image = std::make_unique<QImage>();
            }
            if (task->image->isNull()) {
                qDebug() << "ThreadId:" << QThread::currentThreadId() << "Loading" << task->imageFileName << "..";
                if (!task->image->load(task->imageFileName)) {
                    qWarning() << "Loading" << task->imageFileName << "failed";
                }
            }
            return task;
        }
    };
    stopAsyncImageLoading();
    QPair<int, int> modelRowRange = modelRowRangeForViewportRect(viewport()->rect());
    QList<ImageLoadingTaskSharedPtr> viewportItems;
    viewportItems.reserve(modelRowRange.second - modelRowRange.first);
    for (int row = modelRowRange.first; row < modelRowRange.second; ++row) {
        QModelIndex index = model()->index(row, 0, rootIndex());
        QVariant imageFileNameVariant = model()->data(index);
        QString imageFileName = imageFileNameVariant.toString();
        ImageLoadingTask item{ row, imageFileName };
        QImage* ptr = m_imageCache.take(imageFileName);
        if (ptr) {
            item.image.reset(ptr);
        }
        viewportItems << std::make_shared<ImageLoadingTask>(std::move(item));
    }
    QFuture<ImageLoadingTaskSharedPtr> future = QtConcurrent::mapped(viewportItems, ImageLoader{});
    m_imageLoadingFutureWatcher.setFuture(future);
}

void ImageListView::stopAsyncImageLoading()
{
    m_imageLoadingFutureWatcher.cancel();
}

QRect ImageListView::visualRect(const QModelIndex& index) const
{
    if (!index.isValid()) {
        return QRect();
    }
    // по строке модельного индекса вычисляем
    // строку фото
    int r = index.row() / (width() / m_thumb_size);
    // колонку фото
    int c = index.row() % (width() / m_thumb_size);
    // получаем координаты фото в системе координат window
    int x = c * m_thumb_size;
    int y = r * m_thumb_size;
    // переводим в систему координат видового окна
    QRect result{
        x - horizontalOffset(),
        y - verticalOffset(),
        m_thumb_size,
        m_thumb_size
    };
    return result;
}

void ImageListView::scrollTo(const QModelIndex& index, ScrollHint hint)
{
    Q_UNUSED(hint)

    QRect view = viewport()->rect();
    QRect rect = visualRect(index);

    if (rect.top() < view.top()) {
        verticalScrollBar()->setValue(verticalScrollBar()->value() + rect.top() - view.top());
    } else if (rect.bottom() > view.bottom()) {
        verticalScrollBar()->setValue(
            verticalScrollBar()->value() + qMin(rect.bottom() - view.bottom(), rect.top() - view.top()));
    }
}

QModelIndex ImageListView::indexAt(const QPoint& point) const
{
    if (model()) {
        // point передан в системе координат viewport-a, поэтому
        // переводим координаты точки в систему координат window
        QPoint p{ point.x() + horizontalOffset(), point.y() + verticalOffset() };
        // расчитываем колонку фото
        int c = p.x() / m_thumb_size;
        // расчитываем строку фото
        int r = p.y() / m_thumb_size;
        // переводим в линейный индекс
        int i = r * (width() / m_thumb_size) + c;

        if (i >= 0 && i < model()->rowCount(rootIndex())) {
            return model()->index(i, 0, rootIndex());
        }
    }
    return QModelIndex();
}

QModelIndex ImageListView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(modifiers)

    QModelIndex index = currentIndex();
    if (!index.isValid()) {
        return index;
    }
    int rowCount = model()->rowCount(rootIndex());
    QRect viewRect = viewport()->rect();
    int tileWidth = viewRect.width() / (width() / m_thumb_size);
    int tileHeight = qMin(tileWidth, viewRect.height());
    int viewColumnCount = viewRect.width() / tileWidth;
    int viewRowCount = viewRect.height() / tileHeight;
    int pageOffset = viewColumnCount * viewRowCount;

    int offset = 0;
    switch (cursorAction) {
    case MoveHome:
        offset = -index.row();
        break;
    case MoveEnd:
        offset = qMax(rowCount - index.row() - 1, 0);
        break;
    case MovePageDown:
        offset += pageOffset;
        break;
    case MovePageUp:
        offset -= pageOffset;
        break;
    case MovePrevious:
    case MoveLeft:
        offset = -1;
        break;
    case MoveNext:
    case MoveRight:
        offset = 1;
        break;
    case MoveUp:
        if ((index.row() + 1) > (width() / m_thumb_size)) {
            offset = -(width() / m_thumb_size);
        }
        break;
    case MoveDown:
        if ((index.row() + (width() / m_thumb_size)) < rowCount) {
            offset = +(width() / m_thumb_size);
        }
    }
    return model()->index(qMax(0, qMin(index.row() + offset, rowCount - 1)), index.column(), rootIndex());
}

int ImageListView::horizontalOffset() const
{
    // у нас не будет скроллирования в горизонтальной плоскости
    return 0;
}

int ImageListView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool ImageListView::isIndexHidden(const QModelIndex& index) const
{
    Q_UNUSED(index);
    return false;
}

void ImageListView::setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command)
{
    QPair<int, int> rowRange = modelRowRangeForViewportRect(rect);
    QItemSelection selection;
    int begin = -1;
    int end = -1;
    for (int row = rowRange.first; row < rowRange.second; row++) {
        QModelIndex index = model()->index(row, 0, rootIndex());
        QRect indexRect = visualRect(index);
        if (indexRect.intersects(rect)) {
            if (begin == -1) {
                begin = end = row;
            } else {
                if ((end + 1) == row) {
                    end = row;
                } else {
                    QModelIndex startIndex = model()->index(begin, 0, rootIndex());
                    QModelIndex finishIndex = model()->index(end, 0, rootIndex());
                    QItemSelection continuousSelection(startIndex, finishIndex);
                    selection.merge(continuousSelection, command);
                    begin = end = row;
                }
            }
        }
    }
    if (begin != -1) {
        QModelIndex startIndex = model()->index(begin, 0, rootIndex());
        QModelIndex finishIndex = model()->index(end, 0, rootIndex());
        QItemSelection continuousSelection(startIndex, finishIndex);
        selection.merge(continuousSelection, command);
    }
    selectionModel()->select(selection, command);
}

QRegion ImageListView::visualRegionForSelection(const QItemSelection& selection) const
{
    QModelIndexList list = selection.indexes();
    QRegion region;
    foreach (const QModelIndex& index, list) {
        QRect rect = visualRect(index);
        if (rect.isValid()) {
            region += rect;
        }
    }
    return region;
}

namespace {
void paintOutline(QPainter& painter, const QRect& rect)
{
    QRect r = rect.adjusted(1, 1, -1, -1);
    painter.save();
    painter.drawRect(r);
    painter.restore();
}
}

void ImageListView::paintEvent(QPaintEvent* event)
{
    m_invalidatingModelRows.clear();
    Q_UNUSED(event);
    QList<int> imageIndexList;
    QPair<int, int> rowRange = modelRowRangeForViewportRect(event->rect());
    for (int row = rowRange.first; row < rowRange.second; ++row) {
        imageIndexList.append(row);
    }
    QStylePainter painter(viewport());
    painter.setRenderHints(QPainter::Antialiasing);

    foreach (int row, imageIndexList) {
        QModelIndex index = model()->index(row, 0, rootIndex());
        if (!index.isValid()) {
            continue;
        }
        QRect rect = visualRect(index);
        if (!rect.isValid() || rect.bottom() < 0 || rect.y() > viewport()->height())
            continue;
        QString imageFileName = model()->data(index).toString();
        QImage* ptr = m_imageCache.object(imageFileName);
        if (ptr) {
            QImage* image = ptr;
            QRectF imageRect = image->rect();
            QRectF drawRect = rect.adjusted(2, 2, -2, -24);
            if (imageRect.width() < imageRect.height()) {
                auto delta = (drawRect.width() - drawRect.width() * imageRect.width() / imageRect.height()) / 2.0;
                drawRect.adjust(delta, 0, -delta, 0);
            } else {
                auto delta = (drawRect.height() - drawRect.height() * imageRect.height() / imageRect.width()) / 2.0;
                drawRect.adjust(0, delta, 0, -delta);
            }
            painter.drawImage(drawRect, *image, imageRect, nullptr);
            QRectF rect_label = rect.adjusted(2, m_thumb_size - 24, -2, -2);
            painter.setPen(QPen(QColor("black"), 1));
            painter.drawText(rect_label, Qt::AlignCenter, QFileInfo(imageFileName).fileName());
        } else {
            painter.setPen(QPen(QColor("gray"), 1));
            painter.drawText(rect, Qt::AlignCenter, "Loading...");
        }
        if (selectionModel()->isSelected(index)) {
            painter.setPen(QPen(QColor("red"), 2));
            paintOutline(painter, rect);
        } else if (currentIndex() == index) {
            painter.setPen(QPen(QColor("yellow"), 1));
            paintOutline(painter, rect);
        }
    }
}

void ImageListView::updateGeometries()
{
    int columnCount = (width() / m_thumb_size);
    // получаем прямоугольник, описывающий окно просмотра
    QRect viewportRect = viewport()->rect();
    // получаем ширину окна просмотра
    int viewportWidth = width();
    // получаем ширину вертикальной полосы прокрутки
    int verticalScrollBarWidth = verticalScrollBar()->width();
    // получаем количество строк модели
    int modelRowCount = model()->rowCount(rootIndex());
    // расчитываем число строк в окне модели
    int windowRowCount = modelRowCount / columnCount + ((modelRowCount % columnCount) ? 1 : 0);
    // расчитываем ширину фото в видовом окне
    //int imageWidth = viewportWidth / columnCount;
    int imageWidth = m_thumb_size;
    // расчитываем высоту фото в видовом окне
    int imageHeight = m_thumb_size;
    if (imageHeight) {
        int viewportRowCount = (viewportRect.height() / imageHeight + 1);
        m_imageCache.setMaxCost(viewportRowCount * columnCount * 2);
    }

    // если высоты вида недостаточна для показа модели целиком
    if (windowRowCount * imageHeight > viewportRect.height()) {
        // корректируем ширину окна просмотра, поскольку станет видима полоса прокрутки
        viewportWidth -= verticalScrollBarWidth;
        // расчитываем максимальное смещение вертикальной полосы прокрутки с учетом корректировки
        int verticalScrollBarMaximum = windowRowCount * imageHeight;
        // если после корректировки высоты видового окна достаточно, чтобы вместить модель целиком
        if (verticalScrollBarMaximum < viewportRect.height()) {
            // оставляем один пиксель, чтобы полоса прокрутки осталась видима
            verticalScrollBarMaximum = 1;
        } else {
            // убираем одну страницу
            verticalScrollBarMaximum -= viewportRect.height();
        }
        // настраиваем параметры вертикальной полосы прокрутки
        verticalScrollBar()->setRange(0, verticalScrollBarMaximum);
        verticalScrollBar()->setPageStep(viewportRect.height() / imageHeight * imageHeight);
        verticalScrollBar()->setSingleStep(imageHeight);

    } else {
        // окна просмотра достаточно, чтобы вместить модель целиком
        // поэтому скрываем вертикальную полосу прокрутки
        verticalScrollBar()->setRange(0, 0);
    }
}

void ImageListView::verticalScrollbarValueChanged(int value)
{
    QAbstractItemView::verticalScrollbarValueChanged(value);
    startScrollDelayTimer();
}

void ImageListView::resizeEvent(QResizeEvent* event)
{
    QAbstractItemView::resizeEvent(event);
    startScrollDelayTimer();
}

void ImageListView::setModel(QAbstractItemModel* model)
{
    QAbstractItemView::setModel(model);
}

void ImageListView::reset()
{
    m_imageCache.clear();
    m_invalidatingModelRows.clear();
    QAbstractItemView::reset();
    startScrollDelayTimer();
}
