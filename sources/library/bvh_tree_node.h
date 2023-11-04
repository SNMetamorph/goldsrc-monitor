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

#pragma once
#include "bounding_box.h"

class CBVHTreeNode
{
public:
    CBVHTreeNode();
    CBVHTreeNode(int index, const CBoundingBox &box);
    CBVHTreeNode(int index, const CBoundingBox &box, CBVHTreeNode &parent);
    void SetParent(int index)                        { m_parentNode = index; };
    void SetLeftChild(int index)                     { m_leftChildNode = index; };
    void SetRightChild(int index)                    { m_rightChildNode = index; };
    void SetBoundingBox(const CBoundingBox &box)     { m_boundingBox = box; };
    void SetDescriptionIndex(int value)              { m_descriptionIndex = value; };
    int GetIndex() const                             { return m_index; };
    int GetParent() const                            { return m_parentNode; }
    int GetLeftChild() const                         { return m_leftChildNode; };
    int GetRightChild() const                        { return m_rightChildNode; };
    int GetDescriptionIndex() const                  { return m_descriptionIndex; }
    const CBoundingBox &GetBoundingBox() const       { return m_boundingBox; };
    const vec3_t &GetSize() const                    { return m_boundingBox.GetSize(); };
    bool IsLeaf() const                              { return m_leftChildNode < 0 && m_rightChildNode < 0; };

private:
    int m_index = 0;
    int m_parentNode = -1;
    int m_leftChildNode = -1;
    int m_rightChildNode = -1;
    int m_descriptionIndex = -1;
    CBoundingBox m_boundingBox;
};
