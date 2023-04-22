/*
Copyright (C) 2023 SNMetamorph

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
GNU General Public License for more details.
*/

#include "bvh_tree_node.h"
#include "exception.h"

CBVHTreeNode::CBVHTreeNode()
{
}

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
