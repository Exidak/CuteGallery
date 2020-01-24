#pragma once

#include <QAbstractItemView>
#include <QCache>
#include <QFuture>
#include <QFutureWatcher>
#include <QImage>
#include <QMetaObject>

#include <memory>

class QTimer;

struct ImageLoadingTask {
    int row;
    QString imageFileName;
    std::unique_ptr<QImage> image;
};
using ImageLoadingTaskSharedPtr = std::shared_ptr<ImageLoadingTask>;
using ImageLoadingTaskFutureWatcher = QFutureWatcher<ImageLoadingTaskSharedPtr>;

class ImageListView : public QAbstractItemView {
    Q_OBJECT
public:
    ImageListView(QWidget* parent = Q_NULLPTR);

    // ImageListView interface
public:
    void setThumbnailSize(int size);

    int columnCount() const;
    void setColumnCount(int columnCount);
protected:
    QPair<int, int> modelRowRangeForViewportRect(const QRect& rect);

    void startScrollDelayTimer();
    void stopScrollDelayTimer();
    void startAsyncImageLoading();
    void stopAsyncImageLoading();

    // QAbstractItemView interface
public:
    virtual QRect visualRect(const QModelIndex& index) const override;
    virtual QModelIndex indexAt(const QPoint& point) const override;
    virtual void scrollTo(const QModelIndex& index, ScrollHint hint) override;
    virtual void setModel(QAbstractItemModel* model) override;

public slots:
    virtual void reset() override;

protected:
    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers) override;
    virtual int horizontalOffset() const override;
    virtual int verticalOffset() const override;
    virtual bool isIndexHidden(const QModelIndex& index) const override;
    virtual void setSelection(const QRect& rect, QItemSelectionModel::SelectionFlags command) override;
    virtual QRegion visualRegionForSelection(const QItemSelection& selection) const override;

protected slots:
    virtual void updateGeometries() override;
    virtual void verticalScrollbarValueChanged(int value) override;

protected:
    virtual void paintEvent(QPaintEvent* event) override;
    virtual void resizeEvent(QResizeEvent* event) override;

private:
    //int m_columnCount = 5;
    int m_thumb_size;
    QTimer* m_loadingDelayTimer = nullptr;
    QTimer* m_updatingDelayTimer = nullptr;
    ImageLoadingTaskFutureWatcher m_imageLoadingFutureWatcher;
    QList<int> m_invalidatingModelRows;
    QCache<QString, QImage> m_imageCache;
};
