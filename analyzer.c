#include <stdio.h>
#include <stdlib.h>
#include "cJSON.h"
#include "cJSON.c"
#include <string.h>

typedef struct {
    char *type;
    char *name;
} ParamInfo;

typedef struct {
    ParamInfo *params;
    int count;
} ParamList;

/*각 함수의 if문 카운트*/
int Func_count(char *json_string, long file_size)
{
    int count = 0;
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        printf("JSON 파싱실패 root가 존재하지 않음");
        free(json_string);
        return -1;
    }
    else
    {
        cJSON *ext = cJSON_GetObjectItem(root, "ext");
        if (ext == NULL)
        {
            printf("ext가 존재하지 않음");
            cJSON_Delete(root);
        }

        long arr_size = cJSON_GetArraySize(ext);
        for (int i = 0; i < arr_size; i++)
        {
            cJSON *idx_JSON = cJSON_GetArrayItem(ext, i);
            if (idx_JSON == NULL)
            {
                printf("idx_JSON이 존재하지 않음");
                cJSON_Delete(root);
            }

            cJSON *nodetype = cJSON_GetObjectItem(idx_JSON, "_nodetype");
            if (strcmp(nodetype->valuestring, "FuncDef") == 0)
            {
                count++;
            }
        }
        return count;
    }

    cJSON_Delete(root);
};

ParamList getFuncParams(const cJSON *function)
{
    ParamList paramList;
    paramList.params = NULL;
    paramList.count = 0;

    const cJSON *decl = cJSON_GetObjectItem(function, "decl");
    const cJSON *type = cJSON_GetObjectItem(decl, "type");
    const cJSON *args = cJSON_GetObjectItem(type, "args");
    const cJSON *params = cJSON_GetObjectItem(args, "params");
    if (params == NULL) return paramList;

    int param_count = cJSON_GetArraySize(params);
    paramList.params = (ParamInfo *)malloc(param_count * sizeof(ParamInfo));
    paramList.count = param_count;


    param_count = 0;
    cJSON *param;
    cJSON_ArrayForEach(param, params)
    {
       
        // printf("\tParameter %d:\n", param_count);

        cJSON *param_type = cJSON_GetObjectItem(param, "type");
        cJSON *param_type_nodetype = cJSON_GetObjectItem(param_type, "_nodetype");

        cJSON *param_type_before;
    
        if (param_type_nodetype == NULL) continue;


        int ptrDeclCount = 0;

        cJSON *param_type_names;
        cJSON *param_name;
        char param_type_names_str[256];

        // PtrDecl이 있는 경우 반복적으로 type에 접근
        while (param_type_nodetype != NULL)
        {
            
            if (strcmp(param_type_nodetype->valuestring, "TypeDecl") == 0)
            {
                
                param_type_before = param_type;
                param_type = cJSON_GetObjectItem(param_type, "type");

                 // node type이 Struct 인 경우
                 if (strcmp(cJSON_GetObjectItem(param_type, "_nodetype")->valuestring, "Struct") == 0)
                 {
                    // 구조체 이름 (타입 대신) ex) struct name
                    param_type_names = cJSON_GetObjectItem(param_type, "name");
                    snprintf(param_type_names_str, sizeof(param_type_names_str), "struct %s", param_type_names->valuestring);
                    // 변수 이름
                    param_name = cJSON_GetObjectItem(param_type_before, "declname");
                 }
                 else
                 {
                    param_type_names = cJSON_GetObjectItem(param_type, "names");
                    if (param_type_names == NULL) continue;
                    param_name = cJSON_GetObjectItem(param, "name");
                    if (param_name == NULL) continue;

                    snprintf(param_type_names_str, sizeof(param_type_names_str), "%s", cJSON_GetArrayItem(param_type_names, 0)->valuestring);
                 }
                break;
            }
            else
            {
                if (strcmp(param_type_nodetype->valuestring, "PtrDecl") == 0)
                {
                    ptrDeclCount++;
                }
                
                param_type = cJSON_GetObjectItem(param_type, "type");
                param_type_nodetype = cJSON_GetObjectItem(param_type, "_nodetype");
            }
        }
        
        char ptrDeclStr[ptrDeclCount + 1];
            for (int i = 0; i < ptrDeclCount; i++)
            {
                ptrDeclStr[i] = '*';
            }
            ptrDeclStr[ptrDeclCount] = '\0';
        
        cJSON *param_type_quals = cJSON_GetObjectItem(param_type_before, "quals");  
        cJSON *qual_item = cJSON_GetArrayItem(param_type_quals, 0);

        // strcat(param_type_names_str, " ");
        strcat(param_type_names_str, ptrDeclStr);
        if (qual_item != NULL)
        {
            // printf("\t[Type Name]: %s %s%s %s\n",
            //     qual_item->valuestring,
            //     param_type_names_str,
            //     ptrDeclStr,
            //     cJSON_Print(param_name));
            strcat(param_type_names_str, qual_item->valuestring);
            paramList.params[param_count].type = strdup(param_type_names_str);
            paramList.params[param_count].name = strdup(param_name->valuestring);
        }
        else
        {
            // printf("\t[Type Name]: %s %s%s\n",
            //     param_type_names_str,
            //     ptrDeclStr,
            //     cJSON_Print(param_name));

           
            paramList.params[param_count].type = strdup(param_type_names_str);
            paramList.params[param_count].name = strdup(param_name->valuestring);
        }
       
        free(param_name);
        free(param_type_names);  
        param_count++; 
    }
    return paramList;
}

int IF_Count_Help(cJSON *node)
{
    int count_if = 0;
    cJSON *block_items = cJSON_GetObjectItem(node, "block_items");

    long sizeof_block_items = cJSON_GetArraySize(block_items);

    for (long block_items_idx = 0; block_items_idx < sizeof_block_items; block_items_idx++)
    {
        cJSON *items = cJSON_GetArrayItem(block_items, block_items_idx);
        cJSON *block_items_nodetype = cJSON_GetObjectItem(items, "_nodetype");
        if (strcmp(block_items_nodetype->valuestring, "If") == 0)
        {
            count_if++;
        }
        if (count_if > 0)
        {
            cJSON *iftrue = cJSON_GetObjectItem(items, "iftrue");
            cJSON *iffalse = cJSON_GetObjectItem(items, "iffalse");
            if (!cJSON_IsNull(iftrue))
            {
                count_if += IF_Count_Help(iftrue);
            }
            if (!cJSON_IsNull(iffalse))
            {
                count_if += IF_Count_Help(iffalse);
            }
        }
    }

    return count_if;
}
int elseIF_Count_Help(cJSON *node)
{

    cJSON *block_items = cJSON_GetObjectItem(node, "block_items");
    long sizeof_block_items = cJSON_GetArraySize(block_items);
    int count_if = 0;
    int count_elseif = 0;

    for (long block_items_idx = 0; block_items_idx < sizeof_block_items; block_items_idx++)
    {
        cJSON *items = cJSON_GetArrayItem(block_items, block_items_idx);
        cJSON *block_items_nodetype = cJSON_GetObjectItem(items, "_nodetype");
        if (strcmp(block_items_nodetype->valuestring, "If") == 0)
        {
            cJSON *iffalse = cJSON_GetObjectItem(items, "iffalse");
            if (!cJSON_IsNull(iffalse))
            {
                count_elseif++;
                count_elseif += elseIF_Count_Help(iffalse);
            }
        }
    }

    return count_elseif;
}

char* getReturnType(const cJSON *function){
    cJSON *nodetype = cJSON_GetObjectItem(function, "_nodetype");
    if (strcmp(nodetype->valuestring, "FuncDef") == 0)
    {
        cJSON *decl = cJSON_GetObjectItem(function, "decl");
        cJSON *type = cJSON_GetObjectItem(decl, "type");
        cJSON *nodetype = cJSON_GetObjectItem(type, "_nodetype");
        // return type
        int ptrDeclCount = 0;

        // PtrDecl이 있는 경우 반복적으로 type에 접근
        while (nodetype != NULL)
        {
            
            if (strcmp(nodetype->valuestring, "TypeDecl") == 0)
            {
                type = cJSON_GetObjectItem(type, "type");
                break;
            }
            else
            {
                if (strcmp(nodetype->valuestring, "PtrDecl") == 0)
                {
                    ptrDeclCount++;
                }
                
                type = cJSON_GetObjectItem(type, "type");
                nodetype = cJSON_GetObjectItem(type, "_nodetype");
            }
        }

        // 최종적으로 names에 접근
        cJSON *returnType = cJSON_GetObjectItem(type, "names");

        char ptrDeclStr[ptrDeclCount + 1];
        for (int i = 0; i < ptrDeclCount; i++)
        {
            ptrDeclStr[i] = '*';
        }
        ptrDeclStr[ptrDeclCount] = '\0';

        // printf("return type: %s %s\n",
        //     cJSON_Print(cJSON_GetArrayItem(returnType, 0)),
        //     ptrDeclStr
        //    );
        
        char* combinedReturnType = cJSON_GetArrayItem(returnType, 0)->valuestring;
        strcat(combinedReturnType, ptrDeclStr);

        // snprintf(combinedReturnType, sizeof(combinedReturnType), "%s %s", cJSON_Print(cJSON_GetArrayItem(returnType, 0)), ptrDeclStr);
        return combinedReturnType;

    }
    return NULL;
    
    
}

char* getFuncName(const cJSON *function)

{

    cJSON *nodetype = cJSON_GetObjectItem(function, "_nodetype");
    if (strcmp(nodetype->valuestring, "FuncDef") == 0)
    {
        cJSON *decl = cJSON_GetObjectItem(function, "decl");
        cJSON *name = cJSON_GetObjectItem(decl, "name");
        
        return name->valuestring;
    }
   return NULL;
    
};
/*각 함수의 if문 카운트*/
void IF_Count(cJSON *function)
{
 
    cJSON *nodetype = cJSON_GetObjectItem(function, "_nodetype");
    if (strcmp(nodetype->valuestring, "FuncDef") == 0)
    {
        cJSON *body = cJSON_GetObjectItem(function, "body");
        int count_if = IF_Count_Help(body);
        int count_elif = elseIF_Count_Help(body);
        printf("\nif 개수:      %d\n", count_if);
        printf("else if 개수: %d\n", count_elif);
    }
    
};

void funcCall(FILE *out, const cJSON* node){
    cJSON *name = cJSON_GetObjectItem(node, "name");
    name = cJSON_GetObjectItem(name, "name");
    fprintf(out, "\t%s(", name->valuestring);
    cJSON *args = cJSON_GetObjectItem(node, "args");
    cJSON *exprs = cJSON_GetObjectItem(args, "exprs");
    int expr_count = cJSON_GetArraySize(exprs);
    int expr_index = 0;

    cJSON *expr;
    cJSON_ArrayForEach(expr, exprs)
    {
        if (strcmp(cJSON_GetObjectItem(expr, "_nodetype")->valuestring, "Constant") == 0)
        {
            fprintf(out, "%s", cJSON_GetObjectItem(expr, "value")->valuestring);
        }
        
        expr_index++;
        if (expr_index < expr_count)
        {
            fprintf(out, ", ");
        }
        
    }
    fprintf(out, ");\n");
}

void reverseFuncBody(FILE *out, const cJSON *function){

    cJSON *_nodetype = cJSON_GetObjectItem(function, "_nodetype");
    if (strcmp(_nodetype->valuestring, "FuncDef") == 0)
    {
        const cJSON *body = cJSON_GetObjectItem(function, "body");
        const cJSON *block_items = cJSON_GetObjectItem(body, "block_items");
        if (block_items == NULL) return;
        const cJSON *block_item;
        cJSON_ArrayForEach(block_item, block_items)
        {
            cJSON *block_item_nodetype = cJSON_GetObjectItem(block_item, "_nodetype");
            if (strcmp(block_item_nodetype->valuestring, "FuncCall") == 0)
            {
                funcCall(out, block_item);

            } else if (strcmp(block_item_nodetype->valuestring, "Return") == 0)
            {
                fprintf(out, "\treturn ");
                cJSON *expr = cJSON_GetObjectItem(block_item, "expr");
                if (expr == NULL) continue;
                cJSON *expr_nodetype = cJSON_GetObjectItem(expr, "_nodetype");
                if (strcmp(expr_nodetype->valuestring, "Constant") == 0)
                {
                    fprintf(out, "%s", cJSON_GetObjectItem(expr, "value")->valuestring);
                    fprintf(out, ";\n");
                }
                else if (strcmp(expr_nodetype->valuestring, "FuncCall") == 0)
                {
                    funcCall(out, expr);
                } else {
                    fprintf(out, ";\n");
                }
            }
        }
    }
}

void freeParamList(ParamList *paramList)
{
    for (int i = 0; i < paramList->count; i++)
    {
        free(paramList->params[i].type);
        free(paramList->params[i].name);
    }
    free(paramList->params);
}

int main(int argc, char *argv[])
{
    /*파일 열기*/
    FILE *file = fopen(argv[1], "r");
    if (file == NULL)
    {
        printf("json파일 열기 실패");
        return 0;
    }

    /*json 메모리 할당*/
    fseek(file, 0, SEEK_END);                          // 스트림 위치 끝으로 이동
    long file_size = ftell(file);                      // 스트림 현재 위치 확인
    fseek(file, 0, SEEK_SET);                          // 스트림 위치 처음으로 이동
    char *json_string = (char *)malloc(file_size + 1); // json_string에 메모리 할당

    if (json_string == NULL)
    {
        fclose(file);
        printf("메모리 할당 실패");
        return 0;
    }

    /*읽어오고 문자열 종료설정*/
    fread(json_string, 1, file_size, file); // file에서 1부터 file_size까지 읽고 json_string에 저장
    json_string[file_size] = '\0';          // 마지막 글자를 종료문자로 설정
    fclose(file);
    cJSON *root = cJSON_Parse(json_string);
    if (root == NULL)
    {
        printf("JSON 파싱실패 root가 존재하지 않음");
        free(json_string);
        return 0;
    }
    cJSON *ext = cJSON_GetObjectItem(root, "ext");
    if (ext == NULL)
    {
        printf("ext가 존재하지 않음");
        cJSON_Delete(root);
        return 0;
    }
    long arr_size = cJSON_GetArraySize(ext);

    cJSON *function;
    long idx = 0;
    int Function_count = Func_count(json_string, file_size);
    printf("함수 개수 : %d \n", Function_count);
    cJSON_ArrayForEach(function, ext)
    {
        printf("\n--------------------------------------------\n");
        printf("[%ld]\n", idx);
        idx++;
        char* funcName = getFuncName(function);
        if (funcName == NULL) continue;
        char* returnType = getReturnType(function);
        printf("함수 이름: %s\n",funcName);
        printf("리턴 타입: %s",returnType);
        IF_Count(function);

        
        ParamList paramList = getFuncParams(function);
        for (int i = 0; i < paramList.count; i++)
        {
            printf("파라미터%d 타입, 이름: %s %s\n", i + 1, paramList.params[i].type, paramList.params[i].name);
        }

        // printf("\n<원본 소스코드 복원>\n");
        // printf("%s %s(", returnType, funcName);
        // for (int i = 0; i < paramList.count; i++)
        // {
        //     printf("%s %s", paramList.params[i].type, paramList.params[i].name);
        //     if (i < paramList.count - 1)
        //     {
        //         printf(", ");
        //     }
        // }
        // printf("){\n");
        // reverseFuncBody(function);
        // printf("}\n");

        FILE *output = fopen("ast.c", "a");
        if (output == NULL) {
            printf("ast.c 파일 열기 실패\n");
            return 0;
        }

        fprintf(output, "%s %s(", returnType, funcName);
        for (int i = 0; i < paramList.count; i++) {
            fprintf(output, "%s %s", paramList.params[i].type, paramList.params[i].name);
            if (i < paramList.count - 1) {
                fprintf(output, ", ");
            }
        }
        fprintf(output, "){\n");
    

        reverseFuncBody(output, function);
        

        fprintf(output, "}\n");
        fclose(output);

        free(funcName);
        free(returnType);
        freeParamList(&paramList);
       
    }

    free(json_string);

    return 0;
}
