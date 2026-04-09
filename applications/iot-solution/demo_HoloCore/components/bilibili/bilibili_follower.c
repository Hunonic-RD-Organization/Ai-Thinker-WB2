/**
 * @file bilibili_follower.c
 * @author Seahi-Mo (seahi-mo@foxmail.com)
 * @brief
 * @version 0.1
 * @date 2025-09-22
 *
 * @copyright Ai-Thinker co.,ltd (c) 2025
 *
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "cJSON.h"
#include "bilibili_follower.h"
#include "blog.h"
/**
 * @brief 获取粉丝数
 *  {"code":0,"message":"0","ttl":1,"data":{"mid":355202584,"following":30,"whisper":0,"black":0,"follower":273}}
 * @param user_id
 * @return int
 */
int bilibili_get_fans_count(char *user_id)
{
	if (user_id == NULL)
	{
		return -1;
	}
	blog_info("get bilibili fans user_id:%s\n", user_id);
	char *http_data = https_get_code(user_id);
	if (http_data == NULL)
	{
		free(http_data);
		return -1;
	}
	cJSON *root = cJSON_Parse(http_data); // 检查JSON格式是否正确
	if (root == NULL)
	{
		cJSON_Delete(root);
		free(http_data);
		return -1;
	}
	cJSON *data = cJSON_GetObjectItem(root, "data");
	if (data == NULL)
	{
		cJSON_Delete(root);
		free(http_data);
		return -1;
	}
	cJSON *follower = cJSON_GetObjectItem(data, "follower");
	if (follower == NULL)
	{
		cJSON_Delete(root);
		free(http_data);
		return -1;
	}
	int fans_count = follower->valueint;
	cJSON_Delete(root);
	free(http_data);
	return fans_count;
}