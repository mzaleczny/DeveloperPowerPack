#include "Tilc/Utils/Tree.h"
#include "Tilc/Utils/Math.h"

namespace Tilc
{

    TTreeNode::TTreeNode()
    {
    };

    TTreeNode::TTreeNode(const char* id, const char* sdata, int64_t data)
    {
        if (id) m_Id = id;
        if (sdata) m_Sdata = sdata;
        m_Data = data;
    };

    TTreeNode::~TTreeNode() {
        ClearNodes();
    }

    void TTreeNode::ClearNodes()
    {
        if (m_Nodes.size() > 0)
        {
            TTreeNode* Node;
            for (auto it = m_Nodes.begin(); it != m_Nodes.end(); ++it)
            {
                Node = *it;
                delete Node;
            }
            m_Nodes.clear();
        }
    }



    TTree::TTree()
        : m_Root("Root", "", 0)
    {
        m_ObjectId = C_TTREE;
    }

    TTree::~TTree()
    {
        Clear();
    }

    TTreeNode* TTree::GetNode(TExtString Path)
    {
        size_t Length;
        if (Path != "")
        {
            Path.RTrim('/');
        }
        std::vector<TExtString> Nodes;
        Path.Explode('/', Nodes);
        Length = Nodes.size();
        if (Length == 0)
        {
            return nullptr;
        }

        TExtString Current;
        size_t j;

        TTreeNode* Node = &m_Root;
        TTreeNode* TmpNode;

        if (Nodes[0] != "Root" && Nodes[0] != "")
        {
            return nullptr;
        }

        for (size_t i = 1; i < Length; i++)
        {
            bool FoundChildNode = false;
            Current = Nodes[i];
            if (Current == "")
            {
                return nullptr;
            }
            size_t CurrentNodeSize = Node->m_Nodes.size();
            if (CurrentNodeSize == 0)
            {
                return nullptr;
            }
            for (auto it = Node->m_Nodes.begin(); it != Node->m_Nodes.end(); ++it)
            {
                if ((*it)->m_Id == Current)
                {
                    Node = *it;
                    FoundChildNode = true;
                    break;
                }
            }
            // jeśli nie znaleziono odpowiedniego noda, to kończymy poszukiwanie
            if (!FoundChildNode)
            {
                return nullptr;
            }
        }

        return Node;
    }


    TTreeNode* TTree::GetParentNode(TExtString Path)
    {
        if (Path.length() > 1)
        {
            Path.RTrim('/');
        }
        if (Path == "/")
        {
            return nullptr;
        }

        TTreeNode* Node = nullptr;
        TExtString ParentPath = Path;
        ParentPath.CutAtLast('/');
        if (ParentPath == "")
        {
            ParentPath = "/";
        }

        Node = GetNode(ParentPath);
        return Node;
    }

    TTreeNode* TTree::AppendNode(TExtString path, int64_t data, const char* id, const char* sdata)
    {
        TTreeNode* Node = GetNode(path);
        return AppendNode(Node, data, id, sdata);
    }

    TTreeNode* TTree::AppendNode(TTreeNode* parent, int64_t data, const char* id, const char* sdata)
    {
        TTreeNode* node = parent;

        if (node)
        {
            TExtString sID = id;
            if (sID.length() == 0)
            {
                int RandomValue = rand();
                sID = "Node";
                sID += RandomValue;
            }
            TTreeNode* newNode = new TTreeNode(sID.c_str(), sdata, data);
            node->m_Nodes.push_back(newNode);
            return newNode;
        }

        return nullptr;
    }

    TTreeNode* TTree::AddAsFirst(TExtString path, int64_t data, const char* id, const char* sdata)
    {
        TTreeNode* node = GetNode(path);
        return AddAsFirst(node, data, id, sdata);
    }

    TTreeNode* TTree::AddAsFirst(TTreeNode* parent, int64_t data, const char* id, const char* sdata)
    {
        TTreeNode* node = parent;

        if (node)
        {
            TExtString sID = id;
            if (sID.length() == 0)
            {
                int RandomValue = rand();
                sID = "Node";
                sID += RandomValue;
            }
            TTreeNode* newNode = new TTreeNode(sID.c_str(), sdata, data);
            node->m_Nodes.push_front(newNode);
            return newNode;
        }

        return NULL;
    }

    TTreeNode* TTree::InsertNode(TExtString path, size_t pos, int64_t data, const char* id, const char* sdata)
    {
        TTreeNode* node = GetNode(path);
        return InsertNode(node, pos, data, id, sdata);
    }

    TTreeNode* TTree::InsertNode(TTreeNode* parent, size_t pos, int64_t data, const char* id, const char* sdata)
    {
        TTreeNode* node = parent;

        if (node)
        {
            TExtString sID = id;
            if (sID.length() == 0)
            {
                int RandomValue = rand();
                sID = "Node";
                sID += RandomValue;
            }
            TTreeNode* newNode = new TTreeNode(sID.c_str(), sdata, data);
            auto it = node->m_Nodes.begin();
            std::advance(it, pos);
            node->m_Nodes.insert(it, newNode);
            return newNode;
        }

        return NULL;
    }

    void TTree::DelNode(TExtString path)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* parentNode = GetParentNode(path);

        if (node)
        {
            if (parentNode)
            {
                parentNode->m_Nodes.remove(node);
            }
            delete node;
        }
    }

    size_t TTree::DelSubNodeWithData(TExtString path, int64_t data)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;
        size_t removed = 0;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end();)
            {
                if ((*it)->m_Data == data)
                {
                    delete *it;
                    it = node->m_Nodes.erase(it);
                    ++removed;
                }
                else
                {
                    ++it;
                }
            }
        }

        return removed;
    }

    size_t TTree::DelSubNodeWithId(TExtString path, TExtString id)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;
        size_t removed = 0;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end();)
            {
                if ((*it)->m_Id == id)
                {
                    delete* it;
                    it = node->m_Nodes.erase(it);
                    ++removed;
                }
                else
                {
                    ++it;
                }
            }
        }

        return removed;
    }

    bool TTree::DelFirstNodeWithData(TExtString path, int64_t data)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;
        size_t removed = 0;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end();)
            {
                if ((*it)->m_Data == data)
                {
                    delete* it;
                    it = node->m_Nodes.erase(it);
                    ++removed;
                    break;
                }
                else
                {
                    ++it;
                }
            }
        }

        return removed == 1;
    }

    bool TTree::DelFirstNodeWithId(TExtString path, TExtString id)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;
        size_t removed = 0;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end();)
            {
                if ((*it)->m_Id == id)
                {
                    delete* it;
                    it = node->m_Nodes.erase(it);
                    ++removed;
                    break;
                }
                else
                {
                    ++it;
                }
            }
        }

        return removed == 1;
    }

    bool TTree::DelNodeAt(TExtString path, size_t pos)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;
        size_t removed = 0;

        if (node)
        {
            if (pos >= 0 && pos < node->m_Nodes.size())
            {
                auto it = node->m_Nodes.begin();
                std::advance(it, pos);
                delete *it;
                node->m_Nodes.erase(it);
                removed++;
            }
        }

        return removed == 1;
    }

    size_t TTree::PosWithData(TExtString path, int64_t data)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;
        size_t foundPos = -1;

        if (node)
        {
            size_t i = 0;
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end(); ++it, ++i)
            {
                if ((*it)->m_Data == data)
                {
                    foundPos = i;
                    break;
                }
            }
        }

        return foundPos;
    }

    size_t TTree::PosWithId(TExtString path, TExtString id)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;
        size_t foundPos = -1;

        if (node)
        {
            size_t i = 0;
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end(); ++it, ++i)
            {
                if ((*it)->m_Id == id)
                {
                    foundPos = i;
                    break;
                }
            }
        }

        return foundPos;
    }

    TExtString TTree::GetNodePath(int64_t data, TTreeNode* StartAtNode)
    {
        TTreeNode* node;
        TExtString path;
        if (StartAtNode)
        {
            node = StartAtNode;
        }
        else
        {
            node = &m_Root;
        }
        TTreeNode* tmpNode;
        TExtString p;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end(); ++it)
            {
                tmpNode = *it;
                if (tmpNode->m_Data == data)
                {
                    path += tmpNode->m_Id;
                    if (!StartAtNode)
                    {
                        path.Prepend("/");
                    }
                    return path;
                }
                p = GetNodePath(data, tmpNode);
                if (p.length() > 0)
                {
                    path += tmpNode->m_Id + "/" + p;
                    if (!StartAtNode)
                    {
                        path.Prepend("/");
                    }
                    return path;
                }
            }
        }

        return path;
    }

    TExtString TTree::GetNodeWithIdPath(TExtString id, TTreeNode* startAtNode)
    {
        TTreeNode* node;
        TExtString path = "";
        if (startAtNode)
        {
            node = startAtNode;
        }
        else
        {
            node = &m_Root;
        }
        TTreeNode* tmpNode;
        TExtString p;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end(); ++it)
            {
                tmpNode = *it;
                if (tmpNode->m_Id == id)
                {
                    path += tmpNode->m_Id;
                    if (!startAtNode)
                    {
                        path.Prepend("/");
                    }
                    return path;
                }
                p = GetNodeWithIdPath(id, tmpNode);
                if (p.length() > 0)
                {
                    path += tmpNode->m_Id + "/" + p;
                    if (!startAtNode)
                    {
                        path.Prepend("/");
                    }
                    return path;
                }
            }
        }

        return path;
    }

    bool TTree::Contains(int64_t data)
    {
        TExtString path = GetNodePath(data);
        return path.length() > 0;
    }

    bool TTree::Contains(TExtString id)
    {
        TExtString path = GetNodeWithIdPath(id);
        return path.length() > 0;
    }

    bool TTree::Contains(TExtString path, int64_t data)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end(); ++it)
            {
                tmpNode = *it;
                if (tmpNode->m_Data == data)
                {
                    return true;
                }
            }
        }

        return false;
    }

    bool TTree::ContainsNodeWithId(TExtString path, TExtString id)
    {
        TTreeNode* node = GetNode(path);
        TTreeNode* tmpNode;

        if (node)
        {
            for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end(); ++it)
            {
                tmpNode = *it;
                if (tmpNode->m_Id == id)
                {
                    return true;
                }
            }
        }

        return false;
    }

    void TTree::Clear()
    {
        m_Root.ClearNodes();
    }

    TExtString TTree::SimplePrintTree()
    {
        return SimplePrintNode(&m_Root, "");
    }

    TExtString TTree::SimplePrintNode(TTreeNode* node, TExtString indent)
    {
        TExtString retval = indent + node->m_Id + "\n";

        if (node->m_Nodes.size() == 0)
        {
            return retval;
        }

        TTreeNode* tmpNode;
        for (auto it = node->m_Nodes.begin(); it != node->m_Nodes.end(); ++it)
        {
            tmpNode = *it;
            retval += SimplePrintNode(tmpNode, indent + "    ");
        }

        return retval;
    }

}