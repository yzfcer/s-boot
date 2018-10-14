/****************************************Copyright (c)**************************************************
**                                       ��  ��  ��  ��
**
**                                       yzfcer@163.com
**
**--------------�ļ���Ϣ--------------------------------------------------------------------------------
**��   ��   ��: wind_tree.h / wind_tree.c
**��   ��   ��: Jason Zhou
**����޸�����: 2012.09.26
**��        ��: �ֵܺ��Ӷ�����νṹ�Ĳ�������
**              
**--------------��ʷ�汾��Ϣ----------------------------------------------------------------------------
** ������: Jason Zhou
** ��  ��: v1.0
** �ա���: 2013.11.27
** �衡��: ԭʼ�汾
**
**--------------��ǰ�汾�޶�----------------------------------------------------------------------------
** �޸���: Jason Zhou
** �ա���: 2013.11.27
** �衡��: 
**
**------------------------------------------------------------------------------------------------------
*******************************************************************************************************/
#include "wind_tree.h"
#include "wind_debug.h"
w_err_t wind_tree_init(w_tree_s *tree)
{
    WIND_ASSERT_RETURN(tree != W_NULL,W_ERR_PTR_NULL);
    tree->parent = W_NULL;
    DLIST_INIT(tree->child_list);
    DNODE_INIT(tree->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_insert_child(w_tree_s *parent,w_tree_s *child)
{
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(child != W_NULL,W_ERR_PTR_NULL);
    child->parent = parent;
    dlist_insert_tail(&parent->child_list,&child->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_insert_brother(w_tree_s *oldbrother,w_tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newbrother != W_NULL,W_ERR_PTR_NULL);
    return wind_tree_insert_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_remove_child(w_tree_s *parent,w_tree_s *child)
{
    w_dnode_s *dnode;
    WIND_ASSERT_RETURN(parent != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(child != W_NULL,W_ERR_PTR_NULL);
    dnode = dlist_remove(&parent->child_list,&child->treenode);
    WIND_ASSERT_RETURN(dnode != W_NULL,W_ERR_INVALID);
    child->parent = W_NULL;
    //DNODE_INIT(child->treenode);
    return W_ERR_OK;
}

w_err_t wind_tree_remove_brother(w_tree_s *oldbrother,w_tree_s *newbrother)
{
    WIND_ASSERT_RETURN(oldbrother != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(newbrother != W_NULL,W_ERR_PTR_NULL);
    WIND_ASSERT_RETURN(oldbrother->parent != W_NULL,W_ERR_PTR_NULL);

    return wind_tree_remove_child(oldbrother->parent,newbrother);
}

w_err_t wind_tree_search(w_tree_s *root,w_tree_s *tree)
{
    w_dnode_s *dnode;
    w_tree_s *subtree;
    if(root == tree)
        return W_ERR_OK;
    WIND_ASSERT_RETURN(root != W_NULL,W_ERR_OK);
    WIND_ASSERT_RETURN(tree != W_NULL,W_ERR_OK);
    foreach_node(dnode,&root->child_list)
    {
        subtree = DLIST_OBJ(dnode,w_tree_s,treenode);
        if(subtree == tree)
            return W_ERR_OK;
        if(W_ERR_OK == wind_tree_search(subtree,tree))
            return W_ERR_OK;
    }
    return W_ERR_FAIL;
}

w_err_t wind_tree_visit(w_tree_s *root,void (*visit)(w_tree_s *tree))
{
    w_dnode_s *dnode;
    w_tree_s *subroot;
    WIND_ASSERT_RETURN(visit != W_NULL,W_ERR_OK);
    if(root)
        visit(root);
    foreach_node(dnode,&root->child_list)
    {
        subroot = DLIST_OBJ(dnode,w_tree_s,treenode);
        wind_tree_visit(subroot,visit);
    }
    return W_ERR_OK;
}


