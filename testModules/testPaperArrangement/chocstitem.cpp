#include "chocstitem.h"

namespace choshigui{

ChoCstItem::ChoCstItem( const QVector<QVariant> &itemDatas, ChoCstItem *parentItem )
{
    m_itemDatas = itemDatas;
    m_childItemList.clear();
    m_cluster = NULL;
    m_isRoot = false;
    m_parentItem = parentItem;
    m_icon = QIcon();
}

ChoCstItem::ChoCstItem( const QIcon &icon, ChoCstItem *parentItem )
{
    m_itemDatas.clear();
    m_childItemList.clear();
    m_cluster = NULL;
    m_isRoot = false;
    m_parentItem = parentItem;
    m_icon = icon;
}

ChoCstItem::~ChoCstItem()
{
    qDeleteAll( m_childItemList );
    m_itemDatas.clear();
    m_cluster = NULL;
    m_parentItem = NULL;
}

QIcon ChoCstItem::getIcon()
{
    return m_icon;
}

void ChoCstItem::setIcon( QIcon icon )
{
    m_icon = icon;
}

ChoCstItem *ChoCstItem::getChildItemAt( int number )
{
    return m_childItemList.value( number );
}

ChoCstItem *ChoCstItem::getChildItemAtLabelNum( int label )
{
    if( !hasChildren() ){
        // TODO DebugMessage;
        return NULL;
    }
    foreach( ChoCstItem *childItem, m_childItemList ){
        if( label == childItem->getLabelNum() )
            return childItem;
    }
    // TODO DebugMessage;
    return NULL;
}

ChoCstItem *ChoCstItem::takeChildItemAt( int row )
{
    ChoCstItem *childItem = m_childItemList.takeAt( row );
    Q_ASSERT( childItem );
    childItem->m_parentItem = 0;
    return childItem;
}

ChoCstItem *ChoCstItem::getParent()
{
    return m_parentItem;
}

int ChoCstItem::getChildItemCount() const
{
    return m_childItemList.size();
}

int ChoCstItem::getColumnCount() const
{
    return m_itemDatas.size();
}

QVariant ChoCstItem::getItemDateAt( int column ) const
{
    return m_itemDatas.value( column );
}

int ChoCstItem::getChildNumber()
{
    if( m_parentItem )
        return m_parentItem->m_childItemList.indexOf( const_cast<ChoCstItem *>(this) );
    return 0;
}

bool ChoCstItem::setItemData( int column, const QVariant &value )
{
    if( column < 0 || column >= m_itemDatas.size() ){
        // TODO DebugMessage;
        return false;
    }
    m_itemDatas[column] = value;
    return true;
}

int ChoCstItem::getRowOfChild( ChoCstItem *childItem )
{
    return m_childItemList.indexOf( childItem );
}

void ChoCstItem::setCluster( choshiresource::ChoCluster *cluster )
{
    m_cluster = cluster;
}

int ChoCstItem::getLabelNum()
{
    return m_cluster->m_labelNum;
}

int ChoCstItem::getMinLabelNum()
{
    if( !hasChildren() )
        return -1;
    int minLabel = DEFAULTMINLABEL;
    for( int i = 0; i < getChildItemCount(); i++ ){
        ChoCstItem *childItem = getChildItemAt( i );
        if( !childItem->hasCluster() )
            continue;
        if( minLabel > childItem->getLabelNum() )
            minLabel = childItem->getLabelNum();
    }
    return minLabel;
}

bool ChoCstItem::hasCluster()
{
    return m_cluster != NULL ? true : false;
}

bool ChoCstItem::hasChildren() const
{
    return !m_childItemList.isEmpty();
}

bool ChoCstItem::isRoot()
{
    return m_isRoot;
}

void ChoCstItem::setRoot()
{
    m_isRoot = true;
}

void ChoCstItem::setNotRoot()
{
    m_isRoot = false;
}

void ChoCstItem::addChild( ChoCstItem *item )
{
    item->m_parentItem = this;
    m_childItemList << item;
}

bool ChoCstItem::insertChild( int row, ChoCstItem *item )
{
    if( row < 0 || row > m_childItemList.size() ){
        //TODO DebugMessage
        return false;
    }
    item->m_parentItem = this;
    m_childItemList.insert( row, item );
    return true;
}

bool ChoCstItem::insertChildren( int position, int count, int columns )
{
    if( position < 0 || position > m_childItemList.size() ){
        // TODO DebugMessage;
        return false;
    }
    for( int row = 0; row < count; row++ ){
        QVector<QVariant> itemDatas( columns );
        ChoCstItem *item = new ChoCstItem( itemDatas, this );
        m_childItemList.insert( position, item );
    }
    return true;
}

bool ChoCstItem::insertColumns( int position, int columns )
{
    if( position < 0 || position > m_itemDatas.size() ){
        // TODO DebugMessage;
        return false;
    }
    for( int column = 0; column < columns; column++ ){
        ChoCstItem *childItem = m_childItemList[column];
        childItem->insertColumns( position, columns );
    }
    return true;
}

bool ChoCstItem::removeChildren( int position, int count )
{
    if( position < 0 || position + count > m_childItemList.size() ){
        // TODO DebugMessage;
        return false;
    }
    for( int row = 0; row < count; row++ ){
        delete m_childItemList.takeAt( position );
    }
    return true;
}

bool ChoCstItem::removeColumns( int position, int columns )
{
    if( position < 0 || position + columns > m_childItemList.size() ){
        // TODO DebugMessage;
        return false;
    }
    for( int column = 0; column < columns; column++ )
        m_itemDatas.remove( position );
    foreach( ChoCstItem *childItem, m_childItemList )
        childItem->removeColumns( position, columns );
    return true;
}

void ChoCstItem::swapChildren( int oldRow, int newRow )
{
    QVariant tempVal;
    ChoCstItem *childItem0 = getChildItemAt( oldRow );
    ChoCstItem *childItem1 = getChildItemAt( newRow );
    tempVal = childItem0->m_itemDatas[oldRow + 1];
    childItem0->m_itemDatas[oldRow + 1] = QVariant();
    childItem0->m_itemDatas[newRow + 1] = tempVal;
    tempVal = childItem1->m_itemDatas[newRow + 1];
    childItem1->m_itemDatas[newRow + 1] = QVariant();
    childItem1->m_itemDatas[oldRow + 1] = tempVal;
    m_childItemList.swap( oldRow, newRow );
}

void ChoCstItem::iconMoveLeft()
{
    QVariant tempVal;
    for( int i = 0; i < m_itemDatas.size(); i++ ){
        tempVal = m_itemDatas[i];
        if( tempVal.canConvert<QIcon>() && ( i-1 ) > 0 ){
            m_itemDatas[i-1] = tempVal;
            m_itemDatas[i] = QVariant();
            return;
        }
    }
}

void ChoCstItem::iconMoveRight()
{
    QVariant tempVal;
    for( int i = 0; i < m_itemDatas.size(); i++ ){
        tempVal = m_itemDatas[i];
        if( tempVal.canConvert<QIcon>() && ( i+1 ) < m_itemDatas.size() ){
            m_itemDatas[i+1] = tempVal;
            m_itemDatas[i] = QVariant();
            return;
        }
    }
}



} /* namespace choshigui */
