/*
 * parse.c - PL/0風言語の再帰下降パーサ
 *
 * 対応構文:
 *   - program 宣言
 *   - begin / end ブロック
 *   - 変数宣言 (var)
 *   - 代入文 (:=)
 *   - 四則演算 (+, -, *, div)
 *   - 比較演算 (=, <>, <, >, <=, >=)
 *   - if 文
 */
#include <stdio.h>
#include <stdlib.h>
#include <getsym.h>
extern TOKEN tok;
extern FILE *infile;
extern FILE *outfile;

public int i = 0;

typedef struct
{
	int addr;
	char v[MAXIDLEN + 1];
} s_entry;
s_entry s_table[32];

void error(char *s);
void statement(void);

void compiler(void)
{
	init_getsym();

	getsym();

	if (tok.attr == RWORD && tok.value == PROGRAM)
	{

		getsym();

		if (tok.attr == IDENTIFIER)
		{

			getsym();

			if (tok.attr == SYMBOL && tok.value == SEMICOLON)
			{

				getsym();

				statement();

				if (tok.attr == SYMBOL && tok.value == PERIOD)
				{
					getsym();
					if (tok.attr == EOF)
					{
						fprintf(stderr, "Parsing Done. No errors found.\n");
						fprintf(outfile, "halt\n");
					}
				}
				else
					error("At the end, a period is required.");
			}
			else
				error("After program name, a semicolon is needed.");
		}
		else
			error("Program identifier is needed.");
	}
	else
		error("At the first, program declaration is required.");
}

/* エラーメッセージに [ERROR] プレフィックスを付加して視認性を向上 */
void error(char *s)
{
	fprintf(stderr, "[ERROR] %s\n", s);
	exit(1);
}

//
// Parser
//

void statement(void)
{
	if (tok.attr == NUMBER)
	{
		fprintf(outfile, "loadi r0,%d\n", tok.value);
		getsym();
		if (tok.attr == SYMBOL && tok.value == PLUS)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				fprintf(outfile, "addi r0,%d\n", tok.value);
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r1,'\\n' \n");
				fprintf(outfile, "writec r1\n");
				getsym();
				statement();
				return;
			}
			else
			{
				error("Number expected after +.");
			}
		}
		else if (tok.attr == SYMBOL && tok.value == MINUS)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				fprintf(outfile, "subi r0,%d\n", tok.value);
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r1,'\\n' \n");
				fprintf(outfile, "writec r1\n");
				getsym();
				statement();
				return;
			}
			else
			{
				error("Number expected after -.");
			}
		}
		else if (tok.attr == SYMBOL && tok.value == TIMES)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				fprintf(outfile, "muli r0,%d\n", tok.value);
				fprintf(outfile, "writed r0\n");
				fprintf(outfile, "loadi r1,'\\n' \n");
				fprintf(outfile, "writec r1\n");
				getsym();
			}
			else
			{
				error("Number expected after *.");
			}
		}
	}

	if (tok.attr == RWORD && tok.value == BEGIN)
	{
		getsym();
		statement();
		if (tok.attr != SYMBOL && tok.value != SEMICOLON)
		{
			error("SEMICOLON expected after statement.");
		}
		while (tok.attr == SYMBOL && tok.value == SEMICOLON)
		{
			getsym();
			statement();
		}
		if (tok.attr == RWORD && tok.value == END)
		{
			getsym();
		}
		else
		{
			error("END expected to match BEGIN.");
		}
	}

	// 第２回課題
	if (tok.attr == IDENTIFIER)
	{
		getsym();
		if (tok.attr == SYMBOL && tok.value == BECOMES)
		{
			getsym();
			expression();
		}
		else
		{
			error("BECOMES expected after identifier.");
		}
	}

	if (tok.attr == RWORD && tok.value == IF)
	{
		getsym();
		condition();
		getsym();
	}
}

// outblock
void outblock(void)
{
	if (tok.attr == RWORD && tok.value == VAR)
	{
		getsym();
	com:
		if (tok.attr == IDENTIFIER)
		{
			strcpy(s_table[i].v, tok.charvalue);
			i++;
			getsym();
			if (tok.attr == SYMBOL && tok.value == COMMA)
			{
				getsym();
				goto com;
			}
			else if (tok.attr == SYMBOL && tok.value == SEMICOLON)
			{
				getsym();
				statement();
			}
		}
	}
}

// expression
void expression(void)
{
	if (tok.attr == NUMBER)
	{
		getsym();
		if (tok.attr == SYMBOL && tok.value == PLUS)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				getsym();
			}
			else if (tok.attr == IDENTIFIER)
			{
				strcpy(s_table[i].v, tok.charvalue);
				i++;
				getsym();
			}
			else
			{
				error("invalid right-hand operand");
			}
		}
		else if (tok.attr == SYMBOL && tok.value == MINUS)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				getsym();
			}
			else if (tok.attr == IDENTIFIER)
			{
				strcpy(s_table[i].v, tok.charvalue);
				i++;
				getsym();
			}
			else
			{
				error("invalid right-hand operand");
			}
		}
		else if (tok.attr == SYMBOL && tok.value == TIMES)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				getsym();
			}
			else if (tok.attr == IDENTIFIER)
			{
				strcpy(s_table[i].v, tok.charvalue);
				i++;
				getsym();
			}
			else
			{
				error("invalid right-hand operand");
			}
		}
		else if (tok.attr == RWORD && tok.value == DIV)
		{
			getsym();
			if (tok.attr == NUMBER)
			{
				getsym();
			}
			else if (tok.attr == IDENTIFIER)
			{
				strcpy(s_table[i].v, tok.charvalue);
				i++;
				getsym();
			}
			else
			{
				error("invalid right-hand operand");
			}
		}
		else
		{
			error("invalid operator in expression");
		}
	}
	else
	{
		error("expression must begin with a number");
	}
}

// condition
void condition(void)
{
	expression();
	getsym();
	if (tok.attr == SYMBOL && tok.value == EQL)
	{
		getsym();
		expression();
	}
	else if (tok.attr == SYMBOL && tok.value == NOTEQL)
	{
		getsym();
		expression();
	}
	else if (tok.attr == SYMBOL && tok.value == LESSTHAN)
	{
		getsym();
		expression();
	}
	else if (tok.attr == SYMBOL && tok.value == GRTRTHAN)
	{
		getsym();
		expression();
	}
	else if (tok.attr == SYMBOL && tok.value == LESSEQL)
	{
		getsym();
		expression();
	}
	else if (tok.attr == SYMBOL && tok.value == GRTREQL)
	{
		getsym();
		expression();
	}
	else
	{
		error("Error: invalid relational operator in condition");
	}
}
