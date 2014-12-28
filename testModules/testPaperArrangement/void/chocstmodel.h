#ifndef CHOCSTMODEL_H
#define CHOCSTMODEL_H

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QVariant>
#include "choresource.h"
#include "chocstitem.h"

namespace choshigui {

class ChoCstModel : public QAbstractItemModel {
  Q_OBJECT
 public:
  explicit ChoCstModel( const QStringList &headers, const QString &itemData, QObject *parent = 0 );
  explicit ChoCstModel( QObject *parent = 0 ) : QAbstractItemModel( parent ) {}
  ~ChoCstModel();

  int rowCount( const QModelIndex &parent = QModelIndex() ) const;
  int columnCount( const QModelIndex &parent = QModelIndex() ) const;
  Qt::ItemFlags flags( const QModelIndex &index );

  QVariant data( const QModelIndex &index, int role ) const;
  QVariant headerData( int section, Qt::Orientation orientation, int role ) const;

  bool setData( const QModelIndex &index, const QVariant &value, int role = Qt::EditRole );
  bool setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role = Qt::EditRole );
  void setResource( choshiresource::ChoResource *resource );
  ChoCstItem *getItemForIndex( const QModelIndex &index ) const;

  QModelIndex insert( const QModelIndex &index );
  bool insertColumn( int column, const QModelIndex &parent = QModelIndex() );
  bool insertColumns( int column, int count, const QModelIndex &parent = QModelIndex() );
  bool removeColumn( int column, const QModelIndex &parent = QModelIndex() );
  bool removeColumns( int column, int count, const QModelIndex &parent = QModelIndex() );

  bool insertRow( int row, const QModelIndex &parent = QModelIndex() );
  bool insertRows( int row, int count, const QModelIndex &parent = QModelIndex() );
  bool removeRow( int row, const QModelIndex &parent = QModelIndex() );
  bool removeRows( int row, int count, const QModelIndex &parent = QModelIndex() );

  QModelIndex index( int row, int  column, const QModelIndex &parent ) const;
  QModelIndex parent( const QModelIndex &child ) const;
  QModelIndex moveUp( const QModelIndex &index );
  QModelIndex moveDown( const QModelIndex &index );
  QModelIndex moveLeft( const QModelIndex &index );
  QModelIndex moveRight( const QModelIndex &index );
  QModelIndex promote( const QModelIndex &index );
  QModelIndex demote( const QModelIndex &index );
  QModelIndex cut( const QModelIndex &index );
  QModelIndex paste( const QModelIndex &index );

  QModelIndex makeIcon( const QModelIndex &index );
  void makeIconPixmap( cv::Mat &srcImage, QPixmap &dstPixmap );

 private:
  void setupModelDatas( const QStringList &lines, ChoCstItem *parentItem );
  QModelIndex moveItem( ChoCstItem *parentItem, int oldRow, int newRow );

 private:
  ChoCstItem *m_rootItem;
  ChoCstItem *m_cutItem;
  choshiresource::ChoResource *m_resource;

};

}

#endif // CHOCSTMODEL_H
