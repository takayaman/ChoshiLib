
#include <QtWidgets>
#include "chocstmodel.h"

namespace choshigui {

ChoCstModel::ChoCstModel( const QStringList &headers, const QString &itemData, QObject *parent )
  : QAbstractItemModel( parent ) {
  QVector<QVariant> rootData;
  foreach( QString header, headers )
    rootData << header;
  m_rootItem = new ChoCstItem( rootData );
  setupModelDatas( itemData.split( QString( "\n" ) ), m_rootItem );
  m_cutItem = NULL;
  m_resource = NULL;
}

ChoCstModel::~ChoCstModel() {
  if( !m_rootItem )
    delete m_rootItem;
  if( !m_cutItem )
    delete m_cutItem;
  m_resource = NULL;
}

QModelIndex ChoCstModel::index( int row, int column, const QModelIndex &parent ) const {
  if( !m_rootItem || row < 0 || column < 0 || column >= m_rootItem->getColumnCount() || !parent.isValid() || 0 ==parent.column() )
    return QModelIndex();
  ChoCstItem *parentItem = getItemForIndex( parent );
  Q_ASSERT( parentItem );
  if( ChoCstItem *item = parentItem->getChildItemAt( row ) )
    return createIndex( row, column, item );
  return QModelIndex();
}

int ChoCstModel::rowCount( const QModelIndex &parent ) const {
  if( parent.isValid() && parent.column() != 0 )
    return 0; // TODO check correct
  ChoCstItem *parentItem = getItemForIndex( parent );
  return parentItem ? parentItem->getChildItemCount() : 0;
}

int ChoCstModel::columnCount( const QModelIndex &parent ) const {
  if( parent.isValid() && parent.column() > 0 )
    return m_rootItem->getColumnCount();
  return 0;
}

Qt::ItemFlags ChoCstModel::flags( const QModelIndex &index ) {
  Qt::ItemFlags itemFlags = QAbstractItemModel::flags( index );
  if( index.isValid() )
    itemFlags = !Qt::ItemIsSelectable | !Qt::ItemIsEnabled;
  return itemFlags;
}

QVariant ChoCstModel::data( const QModelIndex &index, int role ) const {
  if( !m_rootItem || !index.isValid() || index.column() < 0 || index.column() >= columnCount( index ) )
    return QVariant();
  ChoCstItem *item = getItemForIndex( index );
  if( item ) {
    if( Qt::DisplayRole == role ) {
      return item->getItemDateAt( index.column() );
    }
    if( Qt::EditRole == role )
      return tr("empty");
    if( Qt::DecorationRole == role ) {
      if( item->hasChildren() ) {
        if( 0 == index.column() )
          return item->getIcon();
        else if( item->getMinLabelNum() == index.column() - 1 && item->isRoot() )
          return item->getIcon();
      }
      if( item->hasCluster() ) {
        if( item->getLabelNum() == index.column() - 1 )
          return item->getIcon();
        else
          return QVariant();
      } else
        return QVariant();
    }
  }
  return QVariant();
}

QVariant ChoCstModel::headerData( int section, Qt::Orientation orientation, int role ) const {
  /* header is go Horizontal and display string, not editable */
  if( Qt::Horizontal == orientation && Qt::DisplayRole == role )
    return m_rootItem->getItemDateAt( section );
  return QVariant();
}
bool ChoCstModel::setData( const QModelIndex &index, const QVariant &value, int role ) {
  if( !index.isValid() )
    return false;
  ChoCstItem *item = getItemForIndex( index );
  if( item ) {
    if( m_rootItem->getChildItemAt( 0 ) == item )
      item->setRoot();
    if( Qt::EditRole == role || Qt::DisplayRole == role )
      item->setItemData( index.column(), value );
    else if( Qt::DecorationRole == role ) {
      item->setItemData( index.column(), value );
      item->setIcon( qvariant_cast<QIcon>( value ) );
      item->setCluster( m_resource->getClusterAt( index.column() - 1 ) );
    } else
      return false;

    emit dataChanged( index, index );
    return true;
  }
  return false;
}

bool ChoCstModel::setHeaderData( int section, Qt::Orientation orientation, const QVariant &value, int role ) {
  /* header is got Horizontal and display string, not editable */
  if( Qt::Horizontal == orientation && Qt::DisplayRole == role  )
    return false;
  bool result = m_rootItem->setItemData( section, value );
  if( result )
    emit headerDataChanged( orientation, section, section );
  return result;
}

void ChoCstModel::setResource( choshiresource::ChoResource *resource ) {
  m_resource = resource;
}

ChoCstItem *ChoCstModel::getItemForIndex( const QModelIndex &index ) const {
  if( index.isValid() ) {
    ChoCstItem *item = static_cast<ChoCstItem *>( index.internalPointer() );
    if( item )
      return item;
  }
  return m_rootItem;
}

QModelIndex ChoCstModel::insert( const QModelIndex &index ) {
  if( !index.isValid() || !m_cutItem ) {
    /* TODO DebugMessage; */
    return index;
  }
  ChoCstItem *sibling = getItemForIndex( index );
  Q_ASSERT( sibling );
  int row = sibling->getRowOfChild( sibling ) + 1;
  beginInsertRows( index.parent(), row, row );
  sibling->insertChild( row, m_cutItem );
  ChoCstItem *childItem = m_cutItem;
  m_cutItem = NULL;
  endInsertRows();;
  return createIndex( row, 0, childItem );
}

bool ChoCstModel::insertColumn( int column, const QModelIndex &parent ) {
  return insertColumns( column, 1, parent );
}

bool ChoCstModel::insertColumns( int column, int count, const QModelIndex &parent ) {
  bool success;
  beginInsertColumns( parent, column, column + count - 1 );
  success = m_rootItem->insertColumns( column, count );
  endInsertColumns();
  return success;
}

bool ChoCstModel::removeColumn( int column, const QModelIndex &parent ) {
  return removeColumns( column, 1, parent );
}

bool ChoCstModel::removeColumns( int column, int count, const QModelIndex &parent ) {
  bool success;
  beginRemoveColumns( parent, column, column + count - 1 );
  success = m_rootItem->removeColumns( column, count );
  endRemoveColumns();

  /* delete all item data when column is empty */
  if( 0 == m_rootItem->getColumnCount() )
    removeRows( 0, rowCount() );
  return success;
}

bool ChoCstModel::insertRow( int row, const QModelIndex &parent ) {
  return insertRows( row, 1, parent );
}

bool ChoCstModel::insertRows( int row, int count, const QModelIndex &parent ) {
  if( !m_rootItem )
    m_rootItem = new ChoCstItem();

  bool success;
  ChoCstItem *parentItem = parent.isValid() ? getItemForIndex( parent ) : m_rootItem;
  beginInsertRows( parent, row, row + count - 1 );
  success = parentItem->insertChildren( row, count, m_rootItem->getColumnCount() );
  endInsertRows();
  return success;
}

bool ChoCstModel::removeRow( int row, const QModelIndex &parent ) {
  return removeRows( row, 1, parent );
}

bool ChoCstModel::removeRows( int row, int count, const QModelIndex &parent ) {
  if( !m_rootItem ) {
    // TODO DebugMessage;
    return false;
  }
  bool success;
  ChoCstItem *parentItem = parent.isValid() ? getItemForIndex( parent ) : m_rootItem;
  beginRemoveRows( parent, row, row + count - 1 );
  success = parentItem->removeChildren( row, count );
  endRemoveRows();
  return success;
}

QModelIndex ChoCstModel::parent( const QModelIndex &index ) const {
  if( !index.isValid() ) {
    // TODO DebugMessage
    return QModelIndex();
  }
  ChoCstItem *childItem = getItemForIndex( index );
  if( childItem ) {
    ChoCstItem *parentItem = childItem->getParent();
    if( parentItem ) {
      if( m_rootItem == parentItem )
        return QModelIndex();
      ChoCstItem *grandParentItem = parentItem->getParent();
      if( grandParentItem ) {
        int row = grandParentItem->getRowOfChild( parentItem );
        return createIndex( row, 0, parentItem );
      }
    }
  }
  return QModelIndex();
}

QModelIndex ChoCstModel::moveUp( const QModelIndex &index ) {
  if( !index.isValid() || index.row() <= 0 )
    return index;
  ChoCstItem *item = getItemForIndex( index );
  Q_ASSERT( item );
  ChoCstItem *parentItem = item->getParent();
  Q_ASSERT( parentItem );
  return moveItem( parentItem, index.row(), index.row() - 1 );
}

QModelIndex ChoCstModel::moveDown( const QModelIndex &index ) {
  if( !index.isValid() )
    return index;
  ChoCstItem *item = getItemForIndex( index );
  Q_ASSERT( item );
  ChoCstItem *parentItem = item->getParent();
  int newRow = index.row() + 1;
  if( !parentItem || parentItem->getChildItemCount() <= newRow )
    return index;
  return moveItem( parentItem, index.row(), newRow );
}

QModelIndex ChoCstModel::moveLeft( const QModelIndex &index ) {
  if( !index.isValid() || index.column() < 0 )
    return index;
  ChoCstItem *item = getItemForIndex( index );
  Q_ASSERT( item );
  item->iconMoveLeft();
  int row = item->getParent()->getChildItemCount() - 1;
  QModelIndex parentIndex = createIndex( 0, 0, item->getParent() );
  QModelIndex tailIndex = createIndex( row, m_rootItem->getColumnCount(), item );

  int labelNum = item->getLabelNum();
  if( labelNum > 0 )
    m_resource->insertCluster( labelNum, labelNum - 1 );

  emit dataChanged( parentIndex, tailIndex );
  return index;
}

QModelIndex ChoCstModel::moveRight( const QModelIndex &index ) {
  if( !index.isValid() )
    return index;
  ChoCstItem *item = getItemForIndex( index );
  Q_ASSERT( item );
  item->iconMoveRight();
  int row = item->getParent()->getChildItemCount() - 1;
  QModelIndex parentIndex = createIndex( 0, 0, item->getParent() );
  QModelIndex tailIndex = createIndex( row, m_rootItem->getColumnCount(), item );

  int labelNum = item->getLabelNum();
  if( labelNum < m_resource->getClusterSize() - 1 )
    m_resource->insertCluster( labelNum, labelNum + 1 );

  emit dataChanged( parentIndex, tailIndex );
  return index;
}

QModelIndex ChoCstModel::promote( const QModelIndex &index ) {
  if( !index.isValid() )
    return index;
  ChoCstItem *item = getItemForIndex( index );
  Q_ASSERT( item );
  ChoCstItem *parentItem = item->getParent();
  Q_ASSERT( parentItem );
  if( m_rootItem == parentItem )
    return index;
  int row = parentItem->getRowOfChild( item );
  ChoCstItem *childItem = parentItem->takeChildItemAt( row );
  Q_ASSERT( childItem == item );
  ChoCstItem *grandParentItem = parentItem->getParent();
  Q_ASSERT( grandParentItem );
  row = grandParentItem->getRowOfChild( parentItem ) + 1;
  grandParentItem->insertChild( row, childItem );

  row = grandParentItem->getChildItemCount() - 1;
  QModelIndex topLeftIndex = createIndex( 0, 0, grandParentItem );
  QModelIndex bottomRightIndex = createIndex( row, m_rootItem->getColumnCount(), grandParentItem );

  emit dataChanged( topLeftIndex, bottomRightIndex );
  return createIndex( row, 0, childItem );
}

QModelIndex ChoCstModel::demote( const QModelIndex &index ) {
  if( !index.isValid() )
    return index;
  ChoCstItem *item = getItemForIndex( index );
  Q_ASSERT( item );
  ChoCstItem *parentItem = item->getParent();
  Q_ASSERT( parentItem );
  int row = parentItem->getRowOfChild( item );
  if( 0 == row )
    return index;
  ChoCstItem *childItem = parentItem->takeChildItemAt( row );
  Q_ASSERT( childItem == item );
  ChoCstItem *sibling = parentItem->getChildItemAt( row - 1 );
  Q_ASSERT( sibling );
  sibling->addChild( childItem );

  QModelIndex topLeftIndex = createIndex( 0, 0, parentItem );
  QModelIndex bottomRightIndex = createIndex( row, m_rootItem->getColumnCount(), parentItem );

  emit dataChanged( topLeftIndex, bottomRightIndex );
  return createIndex( sibling->getChildItemCount() - 1, 0, childItem );
}

QModelIndex ChoCstModel::cut( const QModelIndex &index ) {
  if( !index.isValid() )
    return index;
  if( !m_cutItem )
    delete m_cutItem;
  m_cutItem = getItemForIndex( index );
  Q_ASSERT( m_cutItem );
  ChoCstItem *parentItem = m_cutItem->getParent();
  Q_ASSERT( parentItem );
  int row = parentItem->getRowOfChild( m_cutItem );

  beginRemoveRows( index.parent(), row, row );
  ChoCstItem *childItem = parentItem->takeChildItemAt( row );
  endRemoveRows();
  Q_ASSERT( childItem == m_cutItem );
  childItem = 0; // ???

  if( 0 < row ) {
    --row;
    return createIndex( row, 0, parentItem->getChildItemAt( row ) );
  }
  ChoCstItem *grandParentItem = parentItem->getParent();
  Q_ASSERT( grandParentItem );
  return createIndex( grandParentItem->getRowOfChild( parentItem ), 0, parentItem );
}

QModelIndex ChoCstModel::paste( const QModelIndex &index ) {
  if( !index.isValid() || !m_cutItem )
    return index;
  ChoCstItem *sibling = getItemForIndex( index );
  Q_ASSERT( sibling );
  ChoCstItem *parentItem = sibling->getParent();
  Q_ASSERT( parentItem );
  int row = parentItem->getRowOfChild( sibling ) + 1;

  beginInsertRows( index.parent(), row, row );
  parentItem->insertChild( row, m_cutItem );
  ChoCstItem *childItem = m_cutItem;
  m_cutItem = 0;
  endInsertRows();;
  return createIndex( row, 0, childItem );
}

QModelIndex ChoCstModel::makeIcon( const QModelIndex &index ) {
  if( !index.isValid() )
    return index;
  ChoCstItem *item = getItemForIndex( index );
  if( !item->hasChildren() )
    return index;
  cv::Mat dstImage = cv::Mat::zeros( m_resource->getImageHeight(), m_resource->getImageWidth(), CV_8UC3 );
  cv::Mat labelImage = cv::Mat::zeros( m_resource->getImageHeight(), m_resource->getImageWidth(), CV_8UC3 );

  int count = item->getChildItemCount();
  int minLabel = DEFAULTMINLABEL;
  for( int i = 0; i < count; i++ ) {
    ChoCstItem *childItem = item->getChildItemAt( i );
    if( minLabel > childItem->getLabelNum() )
      minLabel = childItem->getLabelNum();
    m_resource->copyColorChannelImageMaskedByLabel( &labelImage, childItem->getLabelNum() );
    cv::add( dstImage, labelImage, dstImage );
  }

  QPixmap iconPixmap;
  makeIconPixmap( dstImage, iconPixmap );
  QIcon icon = QIcon( iconPixmap );
  QVariant iconValue( icon );
  bool debug = setData( index, iconValue, Qt::DecorationRole );
  item->setItemData( minLabel + 1, iconValue );

  for( int i = 0; i < count; i++ ) {
    ChoCstItem *childItem = item->getChildItemAt( i );
    m_resource->insertCluster( childItem->getLabelNum(), minLabel );
    minLabel++;
  }
  //TODO check returnval
  return QModelIndex();
}

void ChoCstModel::makeIconPixmap( cv::Mat &srcImage, QPixmap &dstPixmap ) {
  cv::Mat colorImage = cv::Mat( srcImage.size().height, srcImage.size().width, CV_8UC3 );
  colorImage = srcImage.clone();
  cv::cvtColor( colorImage, colorImage, cv::COLOR_BGR2RGB );

  cv::Mat iconMat( 48, 48, colorImage.type() );
  cv::resize( colorImage, iconMat, iconMat.size(), cv::INTER_CUBIC );
  QImage iconImage( iconMat.data, iconMat.size().width, iconMat.size().height, QImage::Format_RGB888 );
  dstPixmap = QPixmap::fromImage( iconImage );
}

void ChoCstModel::setupModelDatas( const QStringList &lines, ChoCstItem *parentItem ) {
  QList<ChoCstItem *> parentItemList;
  QList<int> indentations;
  parentItemList << parentItem;
  indentations << 0;

  int number = 0;
  while( number < lines.size() ) {
    int position = 0;

    /* search space */
    while( position < lines[number].length() ) {
      if( lines[number].mid( position, 1 ) != " " )
        break;
      ++position;
    }
    /* trim after space */
    QString lineData = lines[number].mid( position ).trimmed();

    if( !lineData.isEmpty() ) {
      /* read the column data from the rest of the line */
      QStringList columnStringList = lineData.split( "\t", QString::SkipEmptyParts );
      QVector<QVariant> columnData;
      for( int column = 0; column < columnStringList.count(); column++ ) {
        columnData << columnStringList[column];
      }
      if( position > indentations.last() ) {
        /* the last child of the current parent is now the new parent *
         * unless the current parent has no children
         */
        ChoCstItem *lastParentItem = parentItemList.last();
        if( 0 < lastParentItem->getChildItemCount() ) {
          parentItemList << lastParentItem->getChildItemAt( lastParentItem->getChildItemCount() - 1 );
          indentations << position;
        } else {
          while( position < indentations.last() && parentItemList.size() > 0 ) {
            parentItemList.pop_back();
            indentations.pop_back();
          }
        }

        /* append a new item to the current parent's list of children */
        lastParentItem = parentItemList.last();
        lastParentItem->insertChildren( parentItem->getChildItemCount(), 1, m_rootItem->getColumnCount() );
        for( int column = 0; column < columnData.size(); column++ ) {
          parentItem->getChildItemAt( parentItem->getChildItemCount() - 1 )->setItemData( column, columnData[column] );
        }
      }
      number++;
    }
  }
}

QModelIndex ChoCstModel::moveItem( ChoCstItem *parentItem, int oldRow, int newRow ) {
  Q_ASSERT( (0 <= oldRow) && (parentItem->getChildItemCount() > oldRow) && (0 <= newRow) && (parentItem->getChildItemCount() > newRow) );
  parentItem->swapChildren( oldRow, newRow );
  ChoCstItem *item0 = parentItem->getChildItemAt( oldRow );
  ChoCstItem *item1 = parentItem->getChildItemAt( newRow );
  if( !item0->hasChildren() && !item1->hasChildren() ) {
    int label0 = item0->getLabelNum();
    int label1 = item1->getLabelNum();
    m_resource->insertCluster( label0, label1 );
  } else if( item0->hasChildren() && item1->hasChildren() ) {
    int minLabel = item0->getMinLabelNum();
    if( minLabel < item1->getMinLabelNum() ) {
      for( int i = 0; i < item1->getChildItemCount(); i++ ) {
        ChoCstItem *childItem = item1->getChildItemAt( i );
        m_resource->insertCluster( childItem->getLabelNum(), minLabel );
        minLabel++;
      }
    } else {
      for( int i = 0; i < item1->getChildItemCount(); i++ ) {
        ChoCstItem *childItem = item1->getChildItemAt( i );
        m_resource->insertCluster( childItem->getLabelNum(), minLabel );
        minLabel++;
      }
    }
  }

  QModelIndex oldIndex = createIndex( 0, 0, parentItem );
  QModelIndex newIndex = createIndex( parentItem->getChildItemCount(), parentItem->getColumnCount(), parentItem );

  emit dataChanged( oldIndex, newIndex );
  return newIndex;
}

} /*namespace choshigui */
