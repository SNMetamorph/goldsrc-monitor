#include "bvh_tree_node.h"
#include "exception.h"

CBVHTreeNode::CBVHTreeNode(int index, const CBoundingBox &box)
{
    m_iIndex = index;
    m_BoundingBox = box;
}

CBVHTreeNode::CBVHTreeNode(int index, const CBoundingBox &box, CBVHTreeNode &parent)
{
    m_iIndex = index;
    m_BoundingBox = box;
    m_iParentNode = parent.GetIndex();
    if (parent.GetLeftChild() < 0) {
        parent.SetLeftChild(index);
    }
    else if (parent.GetRightChild() < 0) {
        parent.SetRightChild(index);
    }
    else {
        EXCEPT("trying to set node which already has child nodes");
    }
}
