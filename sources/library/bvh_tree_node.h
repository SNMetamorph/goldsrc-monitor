#pragma once
#include "bounding_box.h"

class CBVHTreeNode
{
public:
    CBVHTreeNode();
    CBVHTreeNode(int index, const CBoundingBox &box);
    CBVHTreeNode(int index, const CBoundingBox &box, CBVHTreeNode &parent);
    inline void SetParent(int index)                        { m_iParentNode = index; };
    inline void SetLeftChild(int index)                     { m_iLeftChildNode = index; };
    inline void SetRightChild(int index)                    { m_iRightChildNode = index; };
    inline void SetBoundingBox(const CBoundingBox &box)     { m_BoundingBox = box; };
    inline void SetDescriptionIndex(int value)              { m_iDescriptionIndex = value; };
    inline int GetIndex() const                             { return m_iIndex; };
    inline int GetParent() const                            { return m_iParentNode; }
    inline int GetLeftChild() const                         { return m_iLeftChildNode; };
    inline int GetRightChild() const                        { return m_iRightChildNode; };
    inline int GetDescriptionIndex() const                  { return m_iDescriptionIndex; }
    inline const CBoundingBox &GetBoundingBox() const       { return m_BoundingBox; };
    inline const vec3_t &GetSize() const                    { return m_BoundingBox.GetSize(); };
    inline bool IsLeaf() const                              { return m_iLeftChildNode < 0 && m_iRightChildNode < 0; };

private:
    int m_iIndex = 0;
    int m_iParentNode = -1;
    int m_iLeftChildNode = -1;
    int m_iRightChildNode = -1;
    int m_iDescriptionIndex = -1;
    CBoundingBox m_BoundingBox;
};
