/**
 * @file signal_handler.h
 * @brief signal_handler defines
 */

#pragma once

#include <iconv.h>

__inline static
bool unicode_to_utf8(char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
	/* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
	 *           IGNORE ：遇到无法转换字符跳过*/
	char* encTo = "UTF-8//IGNORE";
	/* 源编码 */
	char* encFrom = "UNICODE";

	/* 获得转换句柄
	 *@param encTo 目标编码方式
	 *@param encFrom 源编码方式
	 *
	 * */
	iconv_t cd = iconv_open(encTo, encFrom);
	if (cd == (iconv_t)-1)
	{
		perror("iconv_open");
	}

	/* 需要转换的字符串 */
	//printf("inbuf=%s\n", inbuf);

	/* 打印需要转换的字符串的长度 */
	//printf("inlen=%d\n", *inlen);


	/* 由于iconv()函数会修改指针，所以要保存源指针 */
	char* tmpin = inbuf;
	char* tmpout = outbuf;
	size_t insize = *inlen;
	size_t outsize = *outlen;

	/* 进行转换
	 *@param cd iconv_open()产生的句柄
	 *@param srcstart 需要转换的字符串
	 *@param inlen 存放还有多少字符没有转换
	 *@param tempoutbuf 存放转换后的字符串
	 *@param outlen 存放转换后,tempoutbuf剩余的空间
	 *
	 * */
	size_t ret = iconv(cd, &tmpin, inlen, &tmpout, outlen);
	if (ret == -1)
	{
		perror("iconv");
	}

	/* 存放转换后的字符串 */
	//printf("outbuf=%s\n", outbuf);

	//存放转换后outbuf剩余的空间
	//printf("outlen=%d\n", *outlen);

	int i = 0;

	//for (i = 0; i < (outsize - (*outlen)); i++)
	{
		//printf("%2c", outbuf[i]);
		//printf("%x\n", outbuf[i]);
	}

	/* 关闭句柄 */
	iconv_close(cd);

	return 0;
}
__inline static
bool utf8_to_unicode(char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{

	/* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
	 *           IGNORE ：遇到无法转换字符跳过*/
	char* encTo = "UNICODE//IGNORE";
	/* 源编码 */
	char* encFrom = "UTF-8";

	/* 获得转换句柄
	 *@param encTo 目标编码方式
	 *@param encFrom 源编码方式
	 *
	 * */
	iconv_t cd = iconv_open(encTo, encFrom);
	if (cd == (iconv_t)-1)
	{
		perror("iconv_open");
	}

	/* 需要转换的字符串 */
	//printf("inbuf=%s\n", inbuf);

	/* 打印需要转换的字符串的长度 */
	//printf("inlen=%d\n", *inlen);

	/* 由于iconv()函数会修改指针，所以要保存源指针 */
	char* tmpin = inbuf;
	char* tmpout = outbuf;
	size_t insize = *inlen;
	size_t outsize = *outlen;

	/* 进行转换
	 *@param cd iconv_open()产生的句柄
	 *@param srcstart 需要转换的字符串
	 *@param inlen 存放还有多少字符没有转换
	 *@param tempoutbuf 存放转换后的字符串
	 *@param outlen 存放转换后,tempoutbuf剩余的空间
	 *
	 * */
	size_t ret = iconv(cd, &tmpin, inlen, &tmpout, outlen);
	if (ret == -1)
	{
		perror("iconv");
	}

	/* 存放转换后的字符串 */
	//printf("outbuf=%s\n", outbuf);

	//存放转换后outbuf剩余的空间
	//printf("outlen=%d\n", *outlen);

	int i = 0;

	//for (i = 0; i < (outsize - (*outlen)); i++)
	{
		//printf("%2c", outbuf[i]);
		//printf("%x\n", outbuf[i]);
	}

	/* 关闭句柄 */
	iconv_close(cd);

	return 0;
}
__inline static
bool gb2312_to_utf8(char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
	/* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
	 *           IGNORE ：遇到无法转换字符跳过*/
	char* encTo = "UTF-8//IGNORE";
	/* 源编码 */
	char* encFrom = "GB2312";

	/* 获得转换句柄
	 *@param encTo 目标编码方式
	 *@param encFrom 源编码方式
	 *
	 * */
	iconv_t cd = iconv_open(encTo, encFrom);
	if (cd == (iconv_t)-1)
	{
		perror("iconv_open");
	}

	/* 需要转换的字符串 */
	//printf("inbuf=%s\n", inbuf);

	/* 打印需要转换的字符串的长度 */
	//printf("inlen=%d\n", *inlen);


	/* 由于iconv()函数会修改指针，所以要保存源指针 */
	char* tmpin = inbuf;
	char* tmpout = outbuf;
	size_t insize = *inlen;
	size_t outsize = *outlen;

	/* 进行转换
	 *@param cd iconv_open()产生的句柄
	 *@param srcstart 需要转换的字符串
	 *@param inlen 存放还有多少字符没有转换
	 *@param tempoutbuf 存放转换后的字符串
	 *@param outlen 存放转换后,tempoutbuf剩余的空间
	 *
	 * */
	size_t ret = iconv(cd, &tmpin, inlen, &tmpout, outlen);
	if (ret == -1)
	{
		perror("iconv");
	}

	/* 存放转换后的字符串 */
	//printf("outbuf=%s\n", outbuf);

	//存放转换后outbuf剩余的空间
	//printf("outlen=%d\n", *outlen);

	int i = 0;

	//for (i = 0; i < (outsize - (*outlen)); i++)
	{
		//printf("%2c", outbuf[i]);
		//printf("%x\n", outbuf[i]);
	}

	/* 关闭句柄 */
	iconv_close(cd);

	return 0;
}
__inline static
bool utf8_to_gb2312(char* inbuf, size_t* inlen, char* outbuf, size_t* outlen)
{
	/* 目的编码, TRANSLIT：遇到无法转换的字符就找相近字符替换
	 *           IGNORE ：遇到无法转换字符跳过*/
	char* encTo = "GB2312//IGNORE";
	/* 源编码 */
	char* encFrom = "UTF-8";

	/* 获得转换句柄
	 *@param encTo 目标编码方式
	 *@param encFrom 源编码方式
	 *
	 * */
	iconv_t cd = iconv_open(encTo, encFrom);
	if (cd == (iconv_t)-1)
	{
		perror("iconv_open");
	}

	/* 需要转换的字符串 */
	//printf("inbuf=%s\n", inbuf);

	/* 打印需要转换的字符串的长度 */
	//printf("inlen=%d\n", *inlen);

	/* 由于iconv()函数会修改指针，所以要保存源指针 */
	char* tmpin = inbuf;
	char* tmpout = outbuf;
	size_t insize = *inlen;
	size_t outsize = *outlen;

	/* 进行转换
	 *@param cd iconv_open()产生的句柄
	 *@param srcstart 需要转换的字符串
	 *@param inlen 存放还有多少字符没有转换
	 *@param tempoutbuf 存放转换后的字符串
	 *@param outlen 存放转换后,tempoutbuf剩余的空间
	 *
	 * */
	size_t ret = iconv(cd, &tmpin, inlen, &tmpout, outlen);
	if (ret == -1)
	{
		perror("iconv");
	}

	/* 存放转换后的字符串 */
	//printf("outbuf=%s\n", outbuf);

	//存放转换后outbuf剩余的空间
	//printf("outlen=%d\n", *outlen);

	int i = 0;

	//for (i = 0; i < (outsize - (*outlen)); i++)
	{
		//printf("%2c", outbuf[i]);
		//printf("%x\n", outbuf[i]);
	}

	/* 关闭句柄 */
	iconv_close(cd);

	return 0;
}
