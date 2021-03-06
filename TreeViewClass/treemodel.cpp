#include "treeitem.h"
#include "treemodel.h"
#include <QStringList>
#include <QIcon>
#include <QDebug>

TreeModel::TreeModel(QObject *parent)
    : QAbstractItemModel(parent)
{
    update();
}
void TreeModel::update()
{
    rootItem=NULL;
    QList<QVariant> rootData;
    rootData << "门禁监控系统";
    rootItem = new TreeItem(rootData);
}
TreeModel::~TreeModel()
{
    delete rootItem;
}
void TreeModel::setData(QList<TreeViewItem*> treeviewlist)
{
    update();
    treeViewItemList.clear();
    beginResetModel();
    treeViewItemList = treeviewlist;
    setupModelData(rootItem);
    for(int i=0;i<treeViewItemList.count();i++)
    {
        connect(treeViewItemList.at(i),SIGNAL(sigDataChanged(int,int)),this,SLOT(slotDataChanged(int,int)));
    }
    endResetModel();
}
void TreeModel::setXML(QString xmlFile)
{
    //toolList = Util::parse(xmlFile);
    //setupModelData(rootItem);
}

int TreeModel::columnCount(const QModelIndex &parent) const
{
    if (parent.isValid())
        return static_cast<TreeItem*>(parent.internalPointer())->columnCount();
    else
        return rootItem->columnCount();
}

QVariant TreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
        return QVariant();

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    TreeViewItem *treeViewItem;
    for (int i = 0; i < treeViewItemList.count(); ++i)
    {
        treeViewItem = treeViewItemList.at(i);
        if (treeViewItem->mac == item->data(0))
        {
            break;
        }
    }

    if (role == Qt::DisplayRole)
    {
        return treeViewItem->text;
    }
    else if (role == Qt::DecorationRole)
    {
        return treeViewItem->getQicon();
    }
    else if (role == Qt::ToolTipRole)
    {
        return treeViewItem->text;
    }
    else
    {
        return QVariant();
    }
}
TreeItem*  TreeModel::GetTreeItem(const QModelIndex &index)
{
    TreeItem *retTreeItem=NULL;
    if (!index.isValid())
        return retTreeItem;

    retTreeItem = static_cast<TreeItem*>(index.internalPointer());

    return retTreeItem;

}
Equipment*  TreeModel::GetSlave(const QModelIndex &index)
{
    Equipment *retSlave=NULL;
    if (!index.isValid())
        return retSlave;

    TreeItem *item = static_cast<TreeItem*>(index.internalPointer());

    TreeViewItem *treeViewItem;
    for (int i = 0; i < treeViewItemList.count(); ++i)
    {
        treeViewItem = treeViewItemList.at(i);
        if (treeViewItem->mac == item->data(0))
        {
            retSlave=treeViewItem->GetSlave();
            break;
        }
    }

    return retSlave;

}
Qt::ItemFlags TreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
        return 0;

    return QAbstractItemModel::flags(index);
}

QVariant TreeModel::headerData(int section, Qt::Orientation orientation,
                               int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
        return rootItem->data(section);

    return QVariant();
}

QModelIndex TreeModel::index(int row, int column, const QModelIndex &parent)
            const
{
    if (!hasIndex(row, column, parent))
        return QModelIndex();

    TreeItem *parentItem;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    TreeItem *childItem = parentItem->child(row);
    if (childItem)
        return createIndex(row, column, childItem);
    else
        return QModelIndex();
}

QModelIndex TreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
        return QModelIndex();

    TreeItem *childItem = static_cast<TreeItem*>(index.internalPointer());
    TreeItem *parentItem = childItem->parent();

    if (parentItem == rootItem)
        return QModelIndex();

    return createIndex(parentItem->row(), 0, parentItem);
}

int TreeModel::rowCount(const QModelIndex &parent) const
{
    TreeItem *parentItem;
    if (parent.column() > 0)
        return 0;

    if (!parent.isValid())
        parentItem = rootItem;
    else
        parentItem = static_cast<TreeItem*>(parent.internalPointer());

    return parentItem->childCount();
}
/*
 * 查找在treeViewItem是否是item的child.如果是则返回一个TreeItem否则为空
*/
TreeItem * TreeModel::item(TreeItem* item, TreeViewItem* treeViewItem)
{
    TreeItem *treeItem = NULL;
    if (item == NULL)
    {
        return treeItem;
    }

    int parentId = treeViewItem->parentIndexInTree;

    if ((item->data(1) == parentId)&&(item->data(2)== -1))
    {
        treeItem = item;
    }
    else
    {

        for (int j = 0; j < item->childCount(); ++j)
        {
            TreeItem *childItem = item->child(j);
            TreeItem *item2 = this->item(childItem, treeViewItem);
            if (item2)
            {
                treeItem = item2;
                break;
            }
        }

    }

    return treeItem;
}
void TreeModel::slotDataChanged(int index, int parentIndex)
{
    QModelIndex parentIndex1=this->index(parentIndex,0,QModelIndex());
    QModelIndex modelIndex=this->index(index,0,parentIndex1);
    emit dataChanged(modelIndex,modelIndex);
}
QList<TreeViewItem *> *TreeModel::GetData(void)
{
    return &(this->treeViewItemList);
}
QList<TreeViewItem *> TreeModel::GetChildsList(const QModelIndex &idx)
{
    QList<TreeViewItem *> retList;
    retList.clear();//清空列表
    //获取index对应的item
    TreeItem*  tempItem = GetTreeItem(idx);
    //判断是否为空 为空则反回
    if(!tempItem)
        return retList;
    //获取tempItem的index;
    int parentIdx = tempItem->data(1).toInt();
    //获取子列表
    for(int i=0;i<treeViewItemList.count();i++)
    {
        if(parentIdx == treeViewItemList.at(i)->parentIndexInTree)
        {
            retList.append(treeViewItemList.at(i));
        }

    }
    return retList;
}
void TreeModel::setupModelData(TreeItem *parent)
{
    QList<TreeItem*> parents;
    parents.clear();
    parents << parent;

    for (int i = 0; i < treeViewItemList.count(); ++i)
    {
        TreeViewItem* treeViewItem = treeViewItemList.at(i);
        QList<QVariant> columnData;
        columnData << treeViewItem->mac;
        columnData << treeViewItem->index;
        columnData << treeViewItem->parentIndexInTree;
        TreeItem* item = NULL;
        for(int j = 0; j < parents.count(); ++j)
        {

            item = this->item(parents.at(j), treeViewItem);
            if (item)
            {
                break;
            }
        }
        if (item)
        {
            item->appendChild(new TreeItem(columnData, item));
        }
        else
        {
            parents.last()->appendChild(new TreeItem(columnData, parents.last()));
        }
    }
}
