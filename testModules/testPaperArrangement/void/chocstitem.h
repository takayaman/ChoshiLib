#ifndef CHOCSTITAM_H
#define CHOCSTITAM_H

#include <QList>
#include <QVector>
#include <QVariant>
#include <QIcon>
#include "chocluster.h"

namespace choshigui {

#define DEFAULTMINLABEL 100 /* max child num */

class ChoCstItem {
 public:
  explicit ChoCstItem( const QVector<QVariant> &itemDatas, ChoCstItem *parentItem = 0 );
  explicit ChoCstItem( const QIcon &icon = QIcon(), ChoCstItem *parentItem = 0 );
  ~ChoCstItem();

  /* Accessor */
  QIcon getIcon( void );
  void setIcon( QIcon icon );
  ChoCstItem *getChildItemAt( int number );
  ChoCstItem *getChildItemAtLabelNum( int label );
  ChoCstItem *takeChildItemAt( int row );
  ChoCstItem *getParent( void );
  int getChildItemCount( void ) const;
  int getColumnCount( void ) const;

  QVariant getItemDateAt( int column ) const;
  int getChildNumber( void );
  bool setItemData( int column, const QVariant &value );
  int getRowOfChild( ChoCstItem *childItem );
  void setCluster( choshiresource::ChoCluster *cluster );
  int getLabelNum( void );
  int getMinLabelNum( void );

  bool hasCluster( void );
  bool hasChildren( void ) const;
  bool isRoot( void );
  void setRoot( void );
  void setNotRoot( void );

  void addChild( ChoCstItem *item );
  bool insertChild( int row, ChoCstItem *item );
  bool insertChildren( int position, int count, int columns );
  bool insertColumns( int position, int columns );
  bool removeChildren( int position, int count );
  bool removeColumns( int position, int columuns );
  void swapChildren( int oldRow, int newRow );
  void iconMoveLeft();
  void iconMoveRight();



 private:
  bool m_isRoot;
  QList<ChoCstItem *> m_childItemList;
  QVector<QVariant> m_itemDatas;
  ChoCstItem *m_parentItem;
  QIcon m_icon;
  choshiresource::ChoCluster *m_cluster;


}; /* class ChoCSTItem */

} /* namespcae choshigui */

#endif // CHOCSTITAM_H
