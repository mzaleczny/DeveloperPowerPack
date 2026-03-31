#pragma once

#include "Tilc/Globals.h"
#include "Tilc/Utils/Object.h"
#include <list>

namespace Tilc
{

class DECLSPEC TTreeNode
{
public:
    TTreeNode();
    TTreeNode(const char* id, const char* sdata, int64_t data);
    virtual ~TTreeNode();
    void ClearNodes();

    TExtString m_Id{};
    TExtString m_Sdata{};
    int64_t m_Data{};
    std::list<TTreeNode*> m_Nodes{};
};
using PTreeNode = TTreeNode*;


class DECLSPEC TTree : public TObject
{
public:
    TTree();
    virtual ~TTree();

    TTreeNode* GetNode(TExtString Path);
    TTreeNode* GetParentNode(TExtString path);

    TTreeNode* AppendNode(TExtString path, int64_t data, const char* id = "", const char* sdata = "");
    TTreeNode* AppendNode(TTreeNode* parent, int64_t data, const char* id = "", const char* sdata = "");
    
    TTreeNode* AddAsFirst(TExtString path, int64_t data, const char* id = "", const char* sdata = "");
    TTreeNode* AddAsFirst(TTreeNode* parent, int64_t data, const char* id = "", const char* sdata = "");

    TTreeNode* InsertNode(TExtString path, size_t pos, int64_t data, const char* id = "", const char* sdata = "");
    TTreeNode* InsertNode(TTreeNode* parent, size_t pos, int64_t data, const char* id = "", const char* sdata = "");

    void DelNode(TExtString path);

    size_t DelSubNodeWithData(TExtString path, int64_t data);
    size_t DelSubNodeWithId(TExtString path, TExtString id);

    bool DelFirstNodeWithData(TExtString path, int64_t data);
    bool DelFirstNodeWithId(TExtString path, TExtString id);

    bool DelNodeAt(TExtString path, size_t pos);

    size_t PosWithData(TExtString path, int64_t data);
    size_t PosWithId(TExtString path, TExtString id);

    TExtString GetNodePath(int64_t data, TTreeNode *StartAtNode = nullptr);
    TExtString GetNodeWithIdPath(TExtString id, TTreeNode *startAtNode = nullptr);

    bool Contains(int64_t data);
    bool Contains(TExtString id);
    bool Contains(TExtString path, int64_t data);
    bool ContainsNodeWithId(TExtString path, TExtString id);

    void Clear();
    TExtString SimplePrintTree();
private:
    TTreeNode m_Root;
    TExtString SimplePrintNode(TTreeNode *node, TExtString indent);
};

}
