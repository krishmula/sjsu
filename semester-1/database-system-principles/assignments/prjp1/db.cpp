/************************************************************
        Project#1:	CLP & DDL
 ************************************************************/

#include "db.h"
#include <cstdio>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_WIN32) || defined(_WIN64)
#define strcasecmp _stricmp
#endif

int main(int argc, char **argv) {
  int rc = 0;
  token_list *tok_list = NULL, *tok_ptr = NULL, *tmp_tok_ptr = NULL;

  if ((argc != 2) || (strlen(argv[1]) == 0)) {
    printf("Usage: db \"command statement\"\n");
    return 1;
  }

  rc = initialize_tpd_list();

  if (rc) {
    printf("\nError in initialize_tpd_list().\nrc = %d\n", rc);
  } else {
    rc = get_token(argv[1], &tok_list);

    /* Test code */
    tok_ptr = tok_list;
    while (tok_ptr != NULL) {
      printf("%16s \t%d \t %d\n", tok_ptr->tok_string, tok_ptr->tok_class,
             tok_ptr->tok_value);
      tok_ptr = tok_ptr->next;
    }

    if (!rc) {
      rc = do_semantic(tok_list);
    }

    if (rc) {
      tok_ptr = tok_list;
      while (tok_ptr != NULL) {
        if ((tok_ptr->tok_class == error) || (tok_ptr->tok_value == INVALID)) {
          printf("\nError in the string: %s\n", tok_ptr->tok_string);
          printf("rc=%d\n", rc);
          break;
        }
        tok_ptr = tok_ptr->next;
      }
    }

    /* Whether the token list is valid or not, we need to free the memory */
    tok_ptr = tok_list;
    while (tok_ptr != NULL) {
      tmp_tok_ptr = tok_ptr->next;
      free(tok_ptr);
      tok_ptr = tmp_tok_ptr;
    }
  }

  return rc;
}

/*************************************************************
        This is a lexical analyzer for simple SQL statements
 *************************************************************/
int get_token(char *command, token_list **tok_list) {
  int rc = 0, i, j;
  char *start, *cur, temp_string[MAX_TOK_LEN];
  bool done = false;

  start = cur = command;
  while (!done) {
    bool found_keyword = false;

    /* This is the TOP Level for each token */
    memset((void *)temp_string, '\0', MAX_TOK_LEN);
    i = 0;

    /* Get rid of all the leading blanks */
    while (*cur == ' ')
      cur++;

    if (cur && isalpha(*cur)) {
      // find valid identifier
      int t_class;
      do {
        temp_string[i++] = *cur++;
      } while ((isalnum(*cur)) || (*cur == '_'));

      if (!(strchr(STRING_BREAK, *cur))) {
        /* If the next char following the keyword or identifier
           is not a blank, (, ), or a comma, then append this
                 character to temp_string, and flag this as an error */
        temp_string[i++] = *cur++;
        add_to_list(tok_list, temp_string, error, INVALID);
        rc = INVALID;
        done = true;
      } else {

        // We have an identifier with at least 1 character
        // Now check if this ident is a keyword
        for (j = 0, found_keyword = false; j < TOTAL_KEYWORDS_PLUS_TYPE_NAMES;
             j++) {
          if ((strcasecmp(keyword_table[j], temp_string) == 0)) {
            found_keyword = true;
            break;
          }
        }

        if (found_keyword) {
          if (KEYWORD_OFFSET + j < K_CREATE)
            t_class = type_name;
          else if (KEYWORD_OFFSET + j >= F_SUM)
            t_class = function_name;
          else
            t_class = keyword;

          add_to_list(tok_list, temp_string, t_class, KEYWORD_OFFSET + j);
        } else {
          if (strlen(temp_string) <= MAX_IDENT_LEN)
            add_to_list(tok_list, temp_string, identifier, IDENT);
          else {
            add_to_list(tok_list, temp_string, error, INVALID);
            rc = INVALID;
            done = true;
          }
        }

        if (!*cur) {
          add_to_list(tok_list, "", terminator, EOC);
          done = true;
        }
      }
    } else if (isdigit(*cur)) {
      // find valid number
      do {
        temp_string[i++] = *cur++;
      } while (isdigit(*cur));

      if (!(strchr(NUMBER_BREAK, *cur))) {
        /* If the next char following the keyword or identifier
           is not a blank or a ), then append this
                 character to temp_string, and flag this as an error */
        temp_string[i++] = *cur++;
        add_to_list(tok_list, temp_string, error, INVALID);
        rc = INVALID;
        done = true;
      } else {
        add_to_list(tok_list, temp_string, constant, INT_LITERAL);

        if (!*cur) {
          add_to_list(tok_list, "", terminator, EOC);
          done = true;
        }
      }
    } else if ((*cur == '(') || (*cur == ')') || (*cur == ',') ||
               (*cur == '*') || (*cur == '=') || (*cur == '<') ||
               (*cur == '>')) {
      /* Catch all the symbols here. Note: no look ahead here. */
      int t_value;
      switch (*cur) {
      case '(':
        t_value = S_LEFT_PAREN;
        break;
      case ')':
        t_value = S_RIGHT_PAREN;
        break;
      case ',':
        t_value = S_COMMA;
        break;
      case '*':
        t_value = S_STAR;
        break;
      case '=':
        t_value = S_EQUAL;
        break;
      case '<':
        t_value = S_LESS;
        break;
      case '>':
        t_value = S_GREATER;
        break;
      }

      temp_string[i++] = *cur++;

      add_to_list(tok_list, temp_string, symbol, t_value);

      if (!*cur) {
        add_to_list(tok_list, "", terminator, EOC);
        done = true;
      }
    } else if (*cur == '\'') {
      /* Find STRING_LITERRAL */
      int t_class;
      cur++;
      do {
        temp_string[i++] = *cur++;
      } while ((*cur) && (*cur != '\''));

      temp_string[i] = '\0';

      if (!*cur) {
        /* If we reach the end of line */
        add_to_list(tok_list, temp_string, error, INVALID);
        rc = INVALID;
        done = true;
      } else /* must be a ' */
      {
        add_to_list(tok_list, temp_string, constant, STRING_LITERAL);
        cur++;
        if (!*cur) {
          add_to_list(tok_list, "", terminator, EOC);
          done = true;
        }
      }
    } else {
      if (!*cur) {
        add_to_list(tok_list, "", terminator, EOC);
        done = true;
      } else {
        /* not a ident, number, or valid symbol */
        temp_string[i++] = *cur++;
        add_to_list(tok_list, temp_string, error, INVALID);
        rc = INVALID;
        done = true;
      }
    }
  }

  return rc;
}

void add_to_list(token_list **tok_list, char *tmp, int t_class, int t_value) {
  token_list *cur = *tok_list;
  token_list *ptr = NULL;

  // printf("%16s \t%d \t %d\n",tmp, t_class, t_value);

  ptr = (token_list *)calloc(1, sizeof(token_list));
  strcpy(ptr->tok_string, tmp);
  ptr->tok_class = t_class;
  ptr->tok_value = t_value;
  ptr->next = NULL;

  if (cur == NULL)
    *tok_list = ptr;
  else {
    while (cur->next != NULL)
      cur = cur->next;

    cur->next = ptr;
  }
  return;
}

int do_semantic(token_list *tok_list) {
  int rc = 0, cur_cmd = INVALID_STATEMENT;
  bool unique = false;
  token_list *cur = tok_list;

  if ((cur->tok_value == K_CREATE) &&
      ((cur->next != NULL) && (cur->next->tok_value == K_TABLE))) {
    printf("CREATE TABLE statement\n");
    cur_cmd = CREATE_TABLE;
    cur = cur->next->next;
  } else if ((cur->tok_value == K_DROP) &&
             ((cur->next != NULL) && (cur->next->tok_value == K_TABLE))) {
    printf("DROP TABLE statement\n");
    cur_cmd = DROP_TABLE;
    cur = cur->next->next;
  } else if ((cur->tok_value == K_LIST) &&
             ((cur->next != NULL) && (cur->next->tok_value == K_TABLE))) {
    printf("LIST TABLE statement\n");
    cur_cmd = LIST_TABLE;
    cur = cur->next->next;
  } else if ((cur->tok_value == K_LIST) &&
             ((cur->next != NULL) && (cur->next->tok_value == K_SCHEMA))) {
    printf("LIST SCHEMA statement\n");
    cur_cmd = LIST_SCHEMA;
    cur = cur->next->next;
  } else if ((cur->tok_value == K_INSERT) &&
             (cur->next && cur->next->tok_value == K_INTO)) {
    printf("INSERT INTO statment\n");
    cur_cmd = INSERT;
    cur = cur->next->next;
  } else if ((cur->tok_value == K_DELETE) && (cur->next != NULL) &&
             (cur->next->tok_value == K_FROM)) {
    printf("DELETE statement\n");
    cur_cmd = DELETE;
    cur = cur->next;
  } else if (cur->tok_value == K_UPDATE) {
    printf("UPDATE Statement\n");
    cur_cmd = UPDATE;
    cur = cur->next;
  } else if (cur->tok_value == K_SELECT) {
    printf("SELECT Statement\n");
    cur_cmd = SELECT;
    cur = cur->next;
  } else {
    printf("Invalid statement\n");
    rc = cur_cmd;
  }

  if (cur_cmd != INVALID_STATEMENT) {
    switch (cur_cmd) {
    case CREATE_TABLE:
      rc = sem_create_table(cur);
      break;
    case DROP_TABLE:
      rc = sem_drop_table(cur);
      break;
    case LIST_TABLE:
      rc = sem_list_tables();
      break;
    case LIST_SCHEMA:
      rc = sem_list_schema(cur);
      break;
    case INSERT:
      rc = sem_insert_into_table(cur);
      break;
    case DELETE:
      rc = sem_delete_from_table(cur);
      break;
    case UPDATE:
      rc = sem_update_table(cur);
      break;
    case SELECT:
      rc = sem_select(cur);
      break;
    default:; /* no action */
    }
  }

  return rc;
}

int sem_create_table(token_list *t_list) {
  int rc = 0;
  token_list *cur;
  // tab_entry collects the table level info (num_columns, cd_offset, etc.), as
  // the function flows.
  tpd_entry tab_entry;
  tpd_entry *new_entry = NULL;
  bool column_done = false;
  int cur_id = 0;
  // col_entry collects column lvl info (col_name, col_id, etc.) for each and
  // every column in a separate index in it's array.
  cd_entry col_entry[MAX_NUM_COL];

  memset(&tab_entry, '\0', sizeof(tpd_entry));
  cur = t_list;
  if ((cur->tok_class != keyword) && (cur->tok_class != identifier) &&
      (cur->tok_class != type_name)) {
    // Error
    rc = INVALID_TABLE_NAME;
    cur->tok_value = INVALID;
  } else {
    if ((new_entry = get_tpd_from_list(cur->tok_string)) != NULL) {
      rc = DUPLICATE_TABLE_NAME;
      cur->tok_value = INVALID;
    } else {
      strcpy(tab_entry.table_name, cur->tok_string);
      cur = cur->next;
      if (cur->tok_value != S_LEFT_PAREN) {
        // Error
        rc = INVALID_TABLE_DEFINITION;
        cur->tok_value = INVALID;
      } else {
        memset(&col_entry, '\0', (MAX_NUM_COL * sizeof(cd_entry)));

        /* Now build a set of column entries */
        cur = cur->next;
        do {
          if ((cur->tok_class != keyword) && (cur->tok_class != identifier) &&
              (cur->tok_class != type_name)) {
            // Error
            rc = INVALID_COLUMN_NAME;
            cur->tok_value = INVALID;
          } else {
            int i;
            for (i = 0; i < cur_id; i++) {
              /* make column name case sensitive */
              if (strcmp(col_entry[i].col_name, cur->tok_string) == 0) {
                rc = DUPLICATE_COLUMN_NAME;
                cur->tok_value = INVALID;
                break;
              }
            }

            if (!rc) {
              strcpy(col_entry[cur_id].col_name, cur->tok_string);
              col_entry[cur_id].col_id = cur_id;
              col_entry[cur_id].not_null = false; /* set default */

              cur = cur->next;
              if (cur->tok_class != type_name) {
                // Error
                rc = INVALID_TYPE_NAME;
                cur->tok_value = INVALID;
              } else {
                /* Set the column type here, int or char */
                col_entry[cur_id].col_type = cur->tok_value;
                cur = cur->next;

                if (col_entry[cur_id].col_type == T_INT) {
                  if ((cur->tok_value != S_COMMA) &&
                      (cur->tok_value != K_NOT) &&
                      (cur->tok_value != S_RIGHT_PAREN)) {
                    rc = INVALID_COLUMN_DEFINITION;
                    cur->tok_value = INVALID;
                  } else {
                    col_entry[cur_id].col_len = sizeof(int);

                    if ((cur->tok_value == K_NOT) &&
                        (cur->next->tok_value != K_NULL)) {
                      rc = INVALID_COLUMN_DEFINITION;
                      cur->tok_value = INVALID;
                    } else if ((cur->tok_value == K_NOT) &&
                               (cur->next->tok_value == K_NULL)) {
                      col_entry[cur_id].not_null = true;
                      cur = cur->next->next;
                    }

                    if (!rc) {
                      /* I must have either a comma or right paren */
                      if ((cur->tok_value != S_RIGHT_PAREN) &&
                          (cur->tok_value != S_COMMA)) {
                        rc = INVALID_COLUMN_DEFINITION;
                        cur->tok_value = INVALID;
                      } else {
                        if (cur->tok_value == S_RIGHT_PAREN) {
                          column_done = true;
                        }
                        cur = cur->next;
                      }
                    }
                  }
                } // end of T_INT processing
                else {
                  // It must be char() or varchar()
                  if (cur->tok_value != S_LEFT_PAREN) {
                    rc = INVALID_COLUMN_DEFINITION;
                    cur->tok_value = INVALID;
                  } else {
                    /* Enter char(n) processing */
                    cur = cur->next;

                    if (cur->tok_value != INT_LITERAL) {
                      rc = INVALID_COLUMN_LENGTH;
                      cur->tok_value = INVALID;
                    } else {
                      /* Got a valid integer - convert */
                      col_entry[cur_id].col_len = atoi(cur->tok_string);
                      cur = cur->next;

                      if (cur->tok_value != S_RIGHT_PAREN) {
                        rc = INVALID_COLUMN_DEFINITION;
                        cur->tok_value = INVALID;
                      } else {
                        cur = cur->next;

                        if ((cur->tok_value != S_COMMA) &&
                            (cur->tok_value != K_NOT) &&
                            (cur->tok_value != S_RIGHT_PAREN)) {
                          rc = INVALID_COLUMN_DEFINITION;
                          cur->tok_value = INVALID;
                        } else {
                          if ((cur->tok_value == K_NOT) &&
                              (cur->next->tok_value != K_NULL)) {
                            rc = INVALID_COLUMN_DEFINITION;
                            cur->tok_value = INVALID;
                          } else if ((cur->tok_value == K_NOT) &&
                                     (cur->next->tok_value == K_NULL)) {
                            col_entry[cur_id].not_null = true;
                            cur = cur->next->next;
                          }

                          if (!rc) {
                            /* I must have either a comma or right paren */
                            if ((cur->tok_value != S_RIGHT_PAREN) &&
                                (cur->tok_value != S_COMMA)) {
                              rc = INVALID_COLUMN_DEFINITION;
                              cur->tok_value = INVALID;
                            } else {
                              if (cur->tok_value == S_RIGHT_PAREN) {
                                column_done = true;
                              }
                              cur = cur->next;
                            }
                          }
                        }
                      }
                    } /* end char(n) processing */
                  }
                } /* end char processing */
              }
            } // duplicate column name
          } // invalid column name

          /* If rc=0, then get ready for the next column */
          if (!rc) {
            cur_id++;
          }

        } while ((rc == 0) && (!column_done));

        if ((column_done) && (cur->tok_value != EOC)) {
          rc = INVALID_TABLE_DEFINITION;
          cur->tok_value = INVALID;
        }

        if (!rc) {
          /* Now finished building tpd and add it to the tpd list */
          tab_entry.num_columns = cur_id;
          tab_entry.tpd_size =
              sizeof(tpd_entry) + sizeof(cd_entry) * tab_entry.num_columns;
          tab_entry.cd_offset = sizeof(tpd_entry);
          new_entry = (tpd_entry *)calloc(1, tab_entry.tpd_size);

          if (new_entry == NULL) {
            rc = MEMORY_ERROR;
          } else {
            memcpy((void *)new_entry, (void *)&tab_entry, sizeof(tpd_entry));

            memcpy((void *)((char *)new_entry + sizeof(tpd_entry)),
                   (void *)col_entry, sizeof(cd_entry) * tab_entry.num_columns);

            rc = add_tpd_to_list(new_entry);

            if (rc == 0) {
              table_file_header tfh;
              char filename[MAX_IDENT_LEN + 5];
              int record_size = 0;
              int i;

              for (i = 0; i < tab_entry.num_columns; i++) {
                record_size += (1 + col_entry[i].col_len);
              }

              if (record_size % 4) {
                record_size += (4 - (record_size % 4));
              }

              memset(&tfh, '\0', sizeof(table_file_header));
              tfh.file_size = (int)sizeof(table_file_header);
              tfh.record_size = record_size;
              tfh.num_records = 0;
              tfh.record_offset = (int)sizeof(table_file_header);
              tfh.file_header_flag = 0;
              tfh.tpd_ptr = 0;

              memset(filename, '\0', sizeof(filename));
              snprintf(filename, sizeof(filename), "%s.tab",
                       tab_entry.table_name);

              FILE *fhandle = fopen(filename, "wbc");
              if (fhandle == NULL) {
                rc = FILE_OPEN_ERROR;
              } else {
                if (fwrite(&tfh, sizeof(table_file_header), 1, fhandle) != 1) {
                  rc = FILE_OPEN_ERROR;
                }

                fclose(fhandle);
              }
            }

            free(new_entry);
          }
        }
      }
    }
  }
  return rc;
}

int sem_drop_table(token_list *t_list) {
  int rc = 0;
  token_list *cur;
  tpd_entry *tab_entry = NULL;

  cur = t_list;
  if ((cur->tok_class != keyword) && (cur->tok_class != identifier) &&
      (cur->tok_class != type_name)) {
    // Error
    rc = INVALID_TABLE_NAME;
    cur->tok_value = INVALID;
  } else {
    if (cur->next->tok_value != EOC) {
      rc = INVALID_STATEMENT;
      cur->next->tok_value = INVALID;
    } else {
      if ((tab_entry = get_tpd_from_list(cur->tok_string)) == NULL) {
        rc = TABLE_NOT_EXIST;
        cur->tok_value = INVALID;
      } else {
        /* Found a valid tpd, drop it from tpd list */
        rc = drop_tpd_from_list(cur->tok_string);

        if (rc == 0) {
          char filename[MAX_IDENT_LEN + 5];
          memset(filename, '\0', sizeof(filename));
          snprintf(filename, sizeof(filename), "%s.tab", tab_entry->table_name);

          if (remove(filename) != 0) {
            printf("Warning: table dropped from catalogue, but data file %s "
                   "could not be deleted",
                   filename);
          }
        }
      }
    }
  }

  return rc;
}

int sem_list_tables() {
  int rc = 0;
  int num_tables = g_tpd_list->num_tables;
  tpd_entry *cur = &(g_tpd_list->tpd_start);

  if (num_tables == 0) {
    printf("\nThere are currently no tables defined\n");
  } else {
    printf("\nTable List\n");
    printf("*****************\n");
    while (num_tables-- > 0) {
      printf("%s\n", cur->table_name);
      if (num_tables > 0) {
        cur = (tpd_entry *)((char *)cur + cur->tpd_size);
      }
    }
    printf("****** End ******\n");
  }

  return rc;
}

int sem_list_schema(token_list *t_list) {
  int rc = 0;
  token_list *cur;
  tpd_entry *tab_entry = NULL;
  cd_entry *col_entry = NULL;
  char tab_name[MAX_IDENT_LEN + 1];
  char filename[MAX_IDENT_LEN + 1];
  bool report = false;
  FILE *fhandle = NULL;
  int i = 0;

  cur = t_list;

  if (cur->tok_value != K_FOR) {
    rc = INVALID_STATEMENT;
    cur->tok_value = INVALID;
  } else {
    cur = cur->next;

    if ((cur->tok_class != keyword) && (cur->tok_class != identifier) &&
        (cur->tok_class != type_name)) {
      // Error
      rc = INVALID_TABLE_NAME;
      cur->tok_value = INVALID;
    } else {
      memset(filename, '\0', MAX_IDENT_LEN + 1);
      strcpy(tab_name, cur->tok_string);
      cur = cur->next;

      if (cur->tok_value != EOC) {
        if (cur->tok_value == K_TO) {
          cur = cur->next;

          if ((cur->tok_class != keyword) && (cur->tok_class != identifier) &&
              (cur->tok_class != type_name)) {
            // Error
            rc = INVALID_REPORT_FILE_NAME;
            cur->tok_value = INVALID;
          } else {
            if (cur->next->tok_value != EOC) {
              rc = INVALID_STATEMENT;
              cur->next->tok_value = INVALID;
            } else {
              /* We have a valid file name */
              strcpy(filename, cur->tok_string);
              report = true;
            }
          }
        } else {
          /* Missing the TO keyword */
          rc = INVALID_STATEMENT;
          cur->tok_value = INVALID;
        }
      }

      if (!rc) {
        if ((tab_entry = get_tpd_from_list(tab_name)) == NULL) {
          rc = TABLE_NOT_EXIST;
          cur->tok_value = INVALID;
        } else {
          if (report) {
            if ((fhandle = fopen(filename, "a+tc")) == NULL) {
              rc = FILE_OPEN_ERROR;
            }
          }

          if (!rc) {
            /* Find correct tpd, need to parse column and index information */

            /* First, write the tpd_entry information */
            printf("Table PD size            (tpd_size)    = %d\n",
                   tab_entry->tpd_size);
            printf("Table Name               (table_name)  = %s\n",
                   tab_entry->table_name);
            printf("Number of Columns        (num_columns) = %d\n",
                   tab_entry->num_columns);
            printf("Column Descriptor Offset (cd_offset)   = %d\n",
                   tab_entry->cd_offset);
            printf("Table PD Flags           (tpd_flags)   = %d\n\n",
                   tab_entry->tpd_flags);

            if (report) {
              fprintf(fhandle, "Table PD size            (tpd_size)    = %d\n",
                      tab_entry->tpd_size);
              fprintf(fhandle, "Table Name               (table_name)  = %s\n",
                      tab_entry->table_name);
              fprintf(fhandle, "Number of Columns        (num_columns) = %d\n",
                      tab_entry->num_columns);
              fprintf(fhandle, "Column Descriptor Offset (cd_offset)   = %d\n",
                      tab_entry->cd_offset);
              fprintf(fhandle,
                      "Table PD Flags           (tpd_flags)   = %d\n\n",
                      tab_entry->tpd_flags);
            }

            /* Next, write the cd_entry information */
            for (i = 0, col_entry = (cd_entry *)((char *)tab_entry +
                                                 tab_entry->cd_offset);
                 i < tab_entry->num_columns; i++, col_entry++) {
              printf("Column Name   (col_name) = %s\n", col_entry->col_name);
              printf("Column Id     (col_id)   = %d\n", col_entry->col_id);
              printf("Column Type   (col_type) = %d\n", col_entry->col_type);
              printf("Column Length (col_len)  = %d\n", col_entry->col_len);
              printf("Not Null flag (not_null) = %d\n\n", col_entry->not_null);

              if (report) {
                fprintf(fhandle, "Column Name   (col_name) = %s\n",
                        col_entry->col_name);
                fprintf(fhandle, "Column Id     (col_id)   = %d\n",
                        col_entry->col_id);
                fprintf(fhandle, "Column Type   (col_type) = %d\n",
                        col_entry->col_type);
                fprintf(fhandle, "Column Length (col_len)  = %d\n",
                        col_entry->col_len);
                fprintf(fhandle, "Not Null Flag (not_null) = %d\n\n",
                        col_entry->not_null);
              }
            }

            if (report) {
              fflush(fhandle);
              fclose(fhandle);
            }
          } // File open error
        } // Table not exist
      } // no semantic errors
    } // Invalid table name
  } // Invalid statement

  return rc;
}

int sem_insert_into_table(token_list *t_list) {
  int rc = 0;
  token_list *cur_token = t_list;
  tpd_entry *table_descriptor = NULL;
  cd_entry *cur_column = NULL;
  table_file_header tfh;
  FILE *filehandle = NULL;
  char tablename[MAX_IDENT_LEN + 1];
  char filename[MAX_IDENT_LEN + 5];

  int col_count = 0;
  int value_count = 0;
  bool done = false;

  token_list *value_tok[MAX_NUM_COL];

  memset(value_tok, 0, sizeof(value_tok));

  if ((cur_token->tok_class != keyword) &&
      (cur_token->tok_class != identifier) &&
      (cur_token->tok_class != type_name)) {
    rc = INVALID_TABLE_NAME;

    cur_token->tok_value = INVALID;

    return rc;
  }

  memset(tablename, '\0', sizeof(tablename));

  strcpy(tablename, cur_token->tok_string);

  cur_token = cur_token->next;

  if ((table_descriptor = get_tpd_from_list(tablename)) == NULL) {
    rc = TABLE_NOT_EXIST;

    if (cur_token)
      cur_token->tok_value = INVALID;

    return rc;
  }

  if ((cur_token == NULL) || (cur_token->tok_value != K_VALUES)) {
    rc = INVALID_STATEMENT;
    if (cur_token)
      cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;

  if ((cur_token == NULL) || (cur_token->tok_value != S_LEFT_PAREN)) {
    rc = INVALID_STATEMENT;
    if (cur_token)
      cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;

  cur_column =
      (cd_entry *)((char *)table_descriptor + table_descriptor->cd_offset);

  col_count = table_descriptor->num_columns;

  while ((rc == 0) && (!done)) {
    if (cur_token == NULL) {
      rc = INVALID_STATEMENT;
      break;
    }

    if (cur_token->tok_value == S_RIGHT_PAREN) {
      if (value_count != col_count) {
        rc = INVALID_STATEMENT;
      } else {
        done = true;
        cur_token = cur_token->next;
      }
      break;
    }

    if ((cur_token->tok_value == INT_LITERAL) ||
        (cur_token->tok_value == STRING_LITERAL) ||
        (cur_token->tok_value == K_NULL)) {

      if (value_count >= MAX_NUM_COL) {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        break;
      }

      value_tok[value_count++] = cur_token;
      cur_token = cur_token->next;

      if (cur_token == NULL) {
        rc = INVALID_STATEMENT;
        break;
      }

      if (cur_token->tok_value == S_COMMA) {
        if (value_count >= col_count) {
          rc = INVALID_STATEMENT;
          cur_token->tok_value = INVALID;
          break;
        }
        cur_token = cur_token->next;

        if (cur_token == NULL) {
          rc = INVALID_STATEMENT;
          break;
        }
      } else if (cur_token->tok_value == S_RIGHT_PAREN) {
        if (value_count != col_count) {
          rc = INVALID_STATEMENT;
          cur_token->tok_value = INVALID;
          break;
        }
        done = true;
        cur_token = cur_token->next;
        break;
      } else {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        break;
      }
    } else {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      break;
    }

    if (value_count > col_count) {
      rc = INVALID_STATEMENT;
      break;
    }
  }

  if ((rc == 0) && (value_count != col_count)) {
    rc = INVALID_STATEMENT;
  }

  if ((rc == 0) && (cur_token != NULL) && (cur_token->tok_value != EOC)) {
    rc = INVALID_STATEMENT;
    if (cur_token)
      cur_token->tok_value = INVALID;
  }

  if (rc != 0) {
    return rc;
  }

  memset(filename, '\0', sizeof(filename));

  snprintf(filename, sizeof(filename), "%s.tab", tablename);

  if ((filehandle = fopen(filename, "r+b")) == NULL) {
    return FILE_OPEN_ERROR;
  }

  if (fread(&tfh, sizeof(table_file_header), 1, filehandle) != 1) {
    fclose(filehandle);
    return FILE_OPEN_ERROR;
  }

  if (tfh.num_records >= 100) {
    fclose(filehandle);
    return MEMORY_ERROR;
  }

  tfh.tpd_ptr = table_descriptor;

  unsigned char *row_buffer = (unsigned char *)calloc(1, tfh.record_size);

  if (row_buffer == NULL) {
    fclose(filehandle);
    return MEMORY_ERROR;
  }

  unsigned char *wr = row_buffer;

  for (int i = 0; i < col_count; i++) {
    cd_entry *c = &cur_column[i];

    token_list *vtok = value_tok[i];

    if ((c->not_null) && (vtok->tok_value == K_NULL)) {
      rc = INVALID_STATEMENT;
      vtok->tok_value = INVALID;
      break;
    }

    if (c->col_type == T_INT) {
      if (vtok->tok_value == K_NULL) {
        *wr++ = (unsigned char)0;
        memset(wr, 0, sizeof(int));

        wr += sizeof(int);

      } else if (vtok->tok_value == INT_LITERAL) {
        int val = atoi(vtok->tok_string);

        *wr++ = (unsigned char)sizeof(int);

        memcpy(wr, &val, sizeof(int));

        wr += sizeof(int);

      } else {
        rc = INVALID_STATEMENT;
        vtok->tok_value = INVALID;
        break;
      }
    } else if (c->col_type == T_CHAR) {
      if (vtok->tok_value == K_NULL) {
        *wr++ = (unsigned char)0;

        wr += c->col_len;

      } else if (vtok->tok_value == STRING_LITERAL) {
        int slen = (int)strlen(vtok->tok_string);

        if (slen > c->col_len) {
          rc = INVALID_STATEMENT;
          vtok->tok_value = INVALID;
          break;
        }

        *wr++ = (unsigned char)slen;

        memcpy(wr, vtok->tok_string, slen);

        wr += c->col_len;

      } else {
        rc = INVALID_STATEMENT;
        vtok->tok_value = INVALID;
        break;
      }
    } else {
      rc = INVALID_TYPE_NAME;
      break;
    }
  }
  if (rc != 0) {
    free(row_buffer);
    fclose(filehandle);
    return rc;
  }

  if (fseek(filehandle, 0, SEEK_END) != 0) {
    free(row_buffer);
    fclose(filehandle);
    return FILE_OPEN_ERROR;
  }

  if (fwrite(row_buffer, tfh.record_size, 1, filehandle) != 1) {
    free(row_buffer);
    fclose(filehandle);
    return FILE_OPEN_ERROR;
  }

  free(row_buffer);

  tfh.num_records += 1;

  tfh.file_size += tfh.record_size;

  long save_pos = ftell(filehandle);

  if (save_pos == -1L) {
    fclose(filehandle);
    return FILE_OPEN_ERROR;
  }

  if (fseek(filehandle, 0, SEEK_SET) != 0) {
    fclose(filehandle);
    return FILE_OPEN_ERROR;
  }

  tpd_entry *bak = tfh.tpd_ptr;

  tfh.tpd_ptr = 0;

  if (fwrite(&tfh, sizeof(table_file_header), 1, filehandle) != 1) {
    fclose(filehandle);
    return FILE_OPEN_ERROR;
  }

  tfh.tpd_ptr = bak;

  (void)fseek(filehandle, save_pos, SEEK_SET);

  fclose(filehandle);

  return rc;
}

int sem_delete_from_table(token_list *t_list)
{
  int rc = 0;
  token_list *cur_token = t_list;
  tpd_entry *table_descriptor = NULL;
  cd_entry  *col_entry = NULL;

  char table_name[MAX_IDENT_LEN + 1] = {0};
  char filename[MAX_IDENT_LEN + 5]   = {0};

  /* WHERE clause data */
  bool has_where       = false;
  int  where_col_index = -1;
  int  rel_op          = 0;   /* S_EQUAL, S_GREATER, S_LESS */

  bool where_is_null   = false;
  bool where_is_int    = false;
  bool where_is_string = false;

  int  where_int_value = 0;
  char where_str_value[MAX_TOK_LEN] = {0};
  int  where_str_len    = 0;

  /* 1) Parse: FROM table_name [ WHERE ... ] */

  if ((cur_token == NULL) || (cur_token->tok_value != K_FROM))
  {
    rc = INVALID_STATEMENT;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }
  cur_token = cur_token->next;

  if ((cur_token == NULL) ||
      ((cur_token->tok_class != keyword) &&
       (cur_token->tok_class != identifier) &&
       (cur_token->tok_class != type_name)))
  {
    rc = INVALID_TABLE_NAME;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  strcpy(table_name, cur_token->tok_string);

  if ((table_descriptor = get_tpd_from_list(table_name)) == NULL)
  {
    rc = TABLE_NOT_EXIST;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;  /* after table name */

  /* Optional WHERE clause */
  if ((cur_token != NULL) && (cur_token->tok_value == K_WHERE))
  {
    has_where = true;
    cur_token = cur_token->next;  /* column_name */

    if ((cur_token == NULL) ||
        ((cur_token->tok_class != keyword) &&
         (cur_token->tok_class != identifier) &&
         (cur_token->tok_class != type_name)))
    {
      rc = INVALID_COLUMN_NAME;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    /* find column in table_descriptor */
    col_entry = (cd_entry*)((char*)table_descriptor + table_descriptor->cd_offset);
    where_col_index = -1;
    for (int i = 0; i < table_descriptor->num_columns; i++)
    {
      if (strcasecmp(col_entry[i].col_name, cur_token->tok_string) == 0)
      {
        where_col_index = i;
        break;
      }
    }
    if (where_col_index < 0)
    {
      rc = INVALID_COLUMN_NAME;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    cur_token = cur_token->next;  /* relational operator */

    if ((cur_token == NULL) ||
        !((cur_token->tok_value == S_EQUAL) ||
          (cur_token->tok_value == S_GREATER) ||
          (cur_token->tok_value == S_LESS)))
    {
      rc = INVALID_STATEMENT;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    rel_op = cur_token->tok_value;
    cur_token = cur_token->next;  /* data_value */

    if (cur_token == NULL)
    {
      rc = INVALID_STATEMENT;
      return rc;
    }

    /* Determine expected type from column definition */
    int col_type = col_entry[where_col_index].col_type;
    int col_len  = col_entry[where_col_index].col_len;

    if (cur_token->tok_value == K_NULL)
    {
      where_is_null   = true;
      where_is_int    = false;
      where_is_string = false;

      /* allow only '=' NULL; other ops don't make sense */
      if (rel_op != S_EQUAL)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }
    }
    else if (cur_token->tok_value == INT_LITERAL)
    {
      where_is_int    = true;
      where_is_null   = false;
      where_is_string = false;

      if (col_type != T_INT)
      {
        rc = INVALID_STATEMENT;   /* type mismatch */
        cur_token->tok_value = INVALID;
        return rc;
      }

      where_int_value = atoi(cur_token->tok_string);
    }
    else if (cur_token->tok_value == STRING_LITERAL)
    {
      where_is_string = true;
      where_is_null   = false;
      where_is_int    = false;

      if (col_type != T_CHAR)
      {
        rc = INVALID_STATEMENT;    /* type mismatch */
        cur_token->tok_value = INVALID;
        return rc;
      }

      memset(where_str_value, '\0', sizeof(where_str_value));
      strcpy(where_str_value, cur_token->tok_string);
      where_str_len = (int)strlen(where_str_value);

      if (where_str_len > col_len)
      {
        /* cannot compare with a literal longer than column definition */
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }
    }
    else
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }

    cur_token = cur_token->next;  /* should be EOC */
  }

  /* Expect EOC at the end */
  if ((cur_token != NULL) && (cur_token->tok_value != EOC))
  {
    rc = INVALID_STATEMENT;
    cur_token->tok_value = INVALID;
    return rc;
  }

  /* 2) Open table file and read header */
  table_file_header tfh;
  FILE *fhandle = NULL;

  memset(filename, '\0', sizeof(filename));
  snprintf(filename, sizeof(filename), "%s.tab", table_name);

  if ((fhandle = fopen(filename, "r+b")) == NULL)
  {
    return FILE_OPEN_ERROR;
  }

  if (fread(&tfh, sizeof(table_file_header), 1, fhandle) != 1)
  {
    fclose(fhandle);
    return FILE_OPEN_ERROR;
  }

  tfh.tpd_ptr = table_descriptor;  /* refresh pointer in memory */

  if (tfh.num_records == 0)
  {
    printf("Warning: no rows to delete.\n");
    fclose(fhandle);
    return rc;
  }

  /* 3) Load all records into memory */
  int total_records = tfh.num_records;
  int rec_size      = tfh.record_size;
  int rec_offset    = tfh.record_offset;

  unsigned char *all_recs = (unsigned char*)malloc(rec_size * total_records);
  if (!all_recs)
  {
    fclose(fhandle);
    return MEMORY_ERROR;
  }

  if (fseek(fhandle, rec_offset, SEEK_SET) != 0)
  {
    free(all_recs);
    fclose(fhandle);
    return FILE_OPEN_ERROR;
  }

  if (fread(all_recs, rec_size, total_records, fhandle) != (size_t)total_records)
  {
    free(all_recs);
    fclose(fhandle);
    return FILE_OPEN_ERROR;
  }

  /* Precompute column offsets (for WHERE evaluation) */
  col_entry = (cd_entry*)((char*)table_descriptor + table_descriptor->cd_offset);
  int num_cols = table_descriptor->num_columns;
  int col_offsets[MAX_NUM_COL];

  {
    int off = 0;
    for (int i = 0; i < num_cols; i++)
    {
      col_offsets[i] = off;
      off += 1;  /* len */
      if (col_entry[i].col_type == T_INT)
        off += (int)sizeof(int);
      else
        off += col_entry[i].col_len;
    }
  }

  /* 4) Build new record set with only rows that are NOT deleted */
  unsigned char *new_recs = (unsigned char*)malloc(rec_size * total_records);
  if (!new_recs)
  {
    free(all_recs);
    fclose(fhandle);
    return MEMORY_ERROR;
  }

  int kept = 0;

  for (int r = 0; r < total_records; r++)
  {
    unsigned char *rec = all_recs + r * rec_size;
    bool delete_row = false;

    if (!has_where)
    {
      /* No WHERE ⇒ delete everything */
      delete_row = true;
    }
    else
    {
      /* Evaluate condition on this record */
      cd_entry *col = &col_entry[where_col_index];
      unsigned char *p = rec + col_offsets[where_col_index];
      unsigned char len = *p++;  /* length byte, then payload */

      if (where_is_null)
      {
        /* only '=' NULL is allowed and already checked; match if row is NULL */
        delete_row = (len == 0);
      }
      else if (where_is_int)
      {
        if (col->col_type != T_INT)
        {
          /* shouldn't happen if parsing was correct */
          delete_row = false;
        }
        else if (len == 0)
        {
          /* NULL never satisfies numeric comparison */
          delete_row = false;
        }
        else
        {
          int row_val = 0;
          memcpy(&row_val, p, sizeof(int));

          if (rel_op == S_EQUAL)
            delete_row = (row_val == where_int_value);
          else if (rel_op == S_GREATER)
            delete_row = (row_val >  where_int_value);
          else if (rel_op == S_LESS)
            delete_row = (row_val <  where_int_value);
        }
      }
      else if (where_is_string)
      {
        if (col->col_type != T_CHAR)
        {
          delete_row = false;
        }
        else if (len == 0)
        {
          /* NULL never equals non-null string */
          delete_row = false;
        }
        else
        {
          /* build row string (len bytes) and compare lexicographically */
          char row_str[MAX_TOK_LEN];
          int  cmp_len = (len < (sizeof(row_str)-1)) ? len : (sizeof(row_str)-1);
          memset(row_str, '\0', sizeof(row_str));
          memcpy(row_str, p, cmp_len);

          int cmp = strncmp(row_str, where_str_value,
                            (where_str_len < cmp_len ? where_str_len : cmp_len));

          /* refine comparison: account for different lengths */
          if (cmp == 0)
          {
            if (len == where_str_len)
              cmp = 0;
            else if (len > where_str_len)
              cmp = 1;
            else
              cmp = -1;
          }

          if (rel_op == S_EQUAL)
            delete_row = (cmp == 0);
          else if (rel_op == S_GREATER)
            delete_row = (cmp > 0);
          else if (rel_op == S_LESS)
            delete_row = (cmp < 0);
        }
      }
    }

    if (!delete_row)
    {
      /* keep this record */
      memcpy(new_recs + kept * rec_size, rec, rec_size);
      kept++;
    }
  }

  int deleted = total_records - kept;

  if (deleted == 0)
  {
    printf("Warning: no rows deleted.\n");
    free(all_recs);
    free(new_recs);
    fclose(fhandle);
    return rc;
  }

  /* 5) Rewrite table file with kept records only */

  tfh.num_records = kept;
  tfh.file_size   = (int)(sizeof(table_file_header) + kept * rec_size);

  fclose(fhandle);
  fhandle = fopen(filename, "wbc");
  if (fhandle == NULL)
  {
    free(all_recs);
    free(new_recs);
    return FILE_OPEN_ERROR;
  }

  tpd_entry *backup_tpd = tfh.tpd_ptr;
  tfh.tpd_ptr = 0;

  if (fwrite(&tfh, sizeof(table_file_header), 1, fhandle) != 1)
  {
    fclose(fhandle);
    free(all_recs);
    free(new_recs);
    return FILE_OPEN_ERROR;
  }

  tfh.tpd_ptr = backup_tpd;

  if (kept > 0)
  {
    if (fwrite(new_recs, rec_size, kept, fhandle) != (size_t)kept)
    {
      fclose(fhandle);
      free(all_recs);
      free(new_recs);
      return FILE_OPEN_ERROR;
    }
  }

  fclose(fhandle);
  free(all_recs);
  free(new_recs);

  printf("%d row(s) deleted.\n", deleted);

  return rc;
}

int sem_update_table(token_list *t_list)
{
  int rc = 0;
  token_list *cur_token = t_list;
  tpd_entry *table_descriptor = NULL;
  cd_entry  *col_entry = NULL;

  char table_name[MAX_IDENT_LEN + 1] = {0};
  char filename[MAX_IDENT_LEN + 5]   = {0};

  /* SET clause data */
  int  set_col_index = -1;
  bool set_is_null   = false;
  bool set_is_int    = false;
  bool set_is_string = false;
  int  set_int_value = 0;
  char set_str_value[MAX_TOK_LEN] = {0};
  int  set_str_len   = 0;

  /* WHERE clause data */
  bool has_where       = false;
  int  where_col_index = -1;
  int  rel_op          = 0;

  bool where_is_null   = false;
  bool where_is_int    = false;
  bool where_is_string = false;

  int  where_int_value = 0;
  char where_str_value[MAX_TOK_LEN] = {0};
  int  where_str_len    = 0;

  /* 1) Parse: UPDATE table_name SET column_name = value [ WHERE ... ] */

  if ((cur_token == NULL) ||
      ((cur_token->tok_class != keyword) &&
       (cur_token->tok_class != identifier) &&
       (cur_token->tok_class != type_name)))
  {
    rc = INVALID_TABLE_NAME;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  strcpy(table_name, cur_token->tok_string);

  if ((table_descriptor = get_tpd_from_list(table_name)) == NULL)
  {
    rc = TABLE_NOT_EXIST;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;

  if ((cur_token == NULL) || (cur_token->tok_value != K_SET))
  {
    rc = INVALID_STATEMENT;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }
  cur_token = cur_token->next;

  /* column_name to set */
  if ((cur_token == NULL) ||
      ((cur_token->tok_class != keyword) &&
       (cur_token->tok_class != identifier) &&
       (cur_token->tok_class != type_name)))
  {
    rc = INVALID_COLUMN_NAME;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  col_entry = (cd_entry*)((char*)table_descriptor + table_descriptor->cd_offset);
  set_col_index = -1;
  for (int i = 0; i < table_descriptor->num_columns; i++)
  {
    if (strcasecmp(col_entry[i].col_name, cur_token->tok_string) == 0)
    {
      set_col_index = i;
      break;
    }
  }
  if (set_col_index < 0)
  {
    rc = INVALID_COLUMN_NAME;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;

  /* = operator */
  if ((cur_token == NULL) || (cur_token->tok_value != S_EQUAL))
  {
    rc = INVALID_STATEMENT;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;

  /* value to set */
  if (cur_token == NULL)
  {
    rc = INVALID_STATEMENT;
    return rc;
  }

  int set_col_type = col_entry[set_col_index].col_type;
  int set_col_len  = col_entry[set_col_index].col_len;

  if (cur_token->tok_value == K_NULL)
  {
    if (col_entry[set_col_index].not_null)
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }
    set_is_null   = true;
    set_is_int    = false;
    set_is_string = false;
  }
  else if (cur_token->tok_value == INT_LITERAL)
  {
    if (set_col_type != T_INT)
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }
    set_is_null = false;
    set_is_int  = true;
    set_is_string = false;
    set_int_value = atoi(cur_token->tok_string);
  }
  else if (cur_token->tok_value == STRING_LITERAL)
  {
    if (set_col_type != T_CHAR)
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }
    set_is_null = false;
    set_is_int  = false;
    set_is_string = true;
    set_str_len = (int)strlen(cur_token->tok_string);
    if (set_str_len > set_col_len)
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }
    strcpy(set_str_value, cur_token->tok_string);
  }
  else
  {
    rc = INVALID_STATEMENT;
    cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;

  /* Optional WHERE clause */
  if ((cur_token != NULL) && (cur_token->tok_value == K_WHERE))
  {
    has_where = true;
    cur_token = cur_token->next;

    if ((cur_token == NULL) ||
        ((cur_token->tok_class != keyword) &&
         (cur_token->tok_class != identifier) &&
         (cur_token->tok_class != type_name)))
    {
      rc = INVALID_COLUMN_NAME;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    where_col_index = -1;
    for (int i = 0; i < table_descriptor->num_columns; i++)
    {
      if (strcasecmp(col_entry[i].col_name, cur_token->tok_string) == 0)
      {
        where_col_index = i;
        break;
      }
    }
    if (where_col_index < 0)
    {
      rc = INVALID_COLUMN_NAME;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    cur_token = cur_token->next;

    if ((cur_token == NULL) ||
        !((cur_token->tok_value == S_EQUAL) ||
          (cur_token->tok_value == S_GREATER) ||
          (cur_token->tok_value == S_LESS)))
    {
      rc = INVALID_STATEMENT;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    rel_op = cur_token->tok_value;
    cur_token = cur_token->next;

    if (cur_token == NULL)
    {
      rc = INVALID_STATEMENT;
      return rc;
    }

    int where_col_type = col_entry[where_col_index].col_type;
    int where_col_len  = col_entry[where_col_index].col_len;

    if (cur_token->tok_value == K_NULL)
    {
      where_is_null   = true;
      where_is_int    = false;
      where_is_string = false;

      if (rel_op != S_EQUAL)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }
    }
    else if (cur_token->tok_value == INT_LITERAL)
    {
      if (where_col_type != T_INT)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }
      where_is_null = false;
      where_is_int  = true;
      where_is_string = false;
      where_int_value = atoi(cur_token->tok_string);
    }
    else if (cur_token->tok_value == STRING_LITERAL)
    {
      if (where_col_type != T_CHAR)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }
      where_is_null = false;
      where_is_int  = false;
      where_is_string = true;
      where_str_len = (int)strlen(cur_token->tok_string);
      strcpy(where_str_value, cur_token->tok_string);
    }
    else
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }

    cur_token = cur_token->next;
  }

  /* Expect EOC */
  if ((cur_token != NULL) && (cur_token->tok_value != EOC))
  {
    rc = INVALID_STATEMENT;
    cur_token->tok_value = INVALID;
    return rc;
  }

  /* 2) Open table file and read header */
  table_file_header tfh;
  FILE *fhandle = NULL;

  memset(filename, '\0', sizeof(filename));
  snprintf(filename, sizeof(filename), "%s.tab", table_name);

  if ((fhandle = fopen(filename, "r+b")) == NULL)
  {
    return FILE_OPEN_ERROR;
  }

  if (fread(&tfh, sizeof(table_file_header), 1, fhandle) != 1)
  {
    fclose(fhandle);
    return FILE_OPEN_ERROR;
  }

  tfh.tpd_ptr = table_descriptor;

  if (tfh.num_records == 0)
  {
    printf("Warning: no rows to update.\n");
    fclose(fhandle);
    return rc;
  }

  /* 3) Load all records into memory */
  int total_records = tfh.num_records;
  int rec_size      = tfh.record_size;
  int rec_offset    = tfh.record_offset;

  unsigned char *all_recs = (unsigned char*)malloc(rec_size * total_records);
  if (!all_recs)
  {
    fclose(fhandle);
    return MEMORY_ERROR;
  }

  if (fseek(fhandle, rec_offset, SEEK_SET) != 0)
  {
    free(all_recs);
    fclose(fhandle);
    return FILE_OPEN_ERROR;
  }

  if (fread(all_recs, rec_size, total_records, fhandle) != (size_t)total_records)
  {
    free(all_recs);
    fclose(fhandle);
    return FILE_OPEN_ERROR;
  }

  /* Precompute column offsets */
  int num_cols = table_descriptor->num_columns;
  int col_offsets[MAX_NUM_COL];

  {
    int off = 0;
    for (int i = 0; i < num_cols; i++)
    {
      col_offsets[i] = off;
      off += 1;
      if (col_entry[i].col_type == T_INT)
        off += (int)sizeof(int);
      else
        off += col_entry[i].col_len;
    }
  }

  /* 4) Update matching rows */
  int updated = 0;

  for (int r = 0; r < total_records; r++)
  {
    unsigned char *rec = all_recs + r * rec_size;
    bool match = false;

    if (!has_where)
    {
      match = true;
    }
    else
    {
      cd_entry *wcol = &col_entry[where_col_index];
      unsigned char *p = rec + col_offsets[where_col_index];
      unsigned char len = *p++;

      if (where_is_null)
      {
        match = (len == 0);
      }
      else if (where_is_int)
      {
        if (len == 0)
          match = false;
        else
        {
          int val;
          memcpy(&val, p, sizeof(int));
          if (rel_op == S_EQUAL)
            match = (val == where_int_value);
          else if (rel_op == S_LESS)
            match = (val < where_int_value);
          else if (rel_op == S_GREATER)
            match = (val > where_int_value);
        }
      }
      else if (where_is_string)
      {
        if (len == 0)
          match = false;
        else
        {
          char buf[MAX_TOK_LEN];
          memset(buf, 0, sizeof(buf));
          memcpy(buf, p, len);
          int cmp = strcmp(buf, where_str_value);
          if (rel_op == S_EQUAL)
            match = (cmp == 0);
          else if (rel_op == S_LESS)
            match = (cmp < 0);
          else if (rel_op == S_GREATER)
            match = (cmp > 0);
        }
      }
    }

    if (match)
    {
      /* Update this record */
      unsigned char *p = rec + col_offsets[set_col_index];

      if (set_is_null)
      {
        *p = 0;
      }
      else if (set_is_int)
      {
        *p = (unsigned char)sizeof(int);
        memcpy(p + 1, &set_int_value, sizeof(int));
      }
      else if (set_is_string)
      {
        *p = (unsigned char)set_str_len;
        memset(p + 1, 0, col_entry[set_col_index].col_len);
        memcpy(p + 1, set_str_value, set_str_len);
      }

      updated++;
    }
  }

  if (updated == 0)
  {
    printf("Warning: no rows updated.\n");
    free(all_recs);
    fclose(fhandle);
    return rc;
  }

  /* 5) Rewrite table file with updated records */
  fclose(fhandle);
  fhandle = fopen(filename, "wbc");
  if (fhandle == NULL)
  {
    free(all_recs);
    return FILE_OPEN_ERROR;
  }

  tpd_entry *backup_tpd = tfh.tpd_ptr;
  tfh.tpd_ptr = 0;

  if (fwrite(&tfh, sizeof(table_file_header), 1, fhandle) != 1)
  {
    fclose(fhandle);
    free(all_recs);
    return FILE_OPEN_ERROR;
  }

  tfh.tpd_ptr = backup_tpd;

  if (fwrite(all_recs, rec_size, total_records, fhandle) != (size_t)total_records)
  {
    fclose(fhandle);
    free(all_recs);
    return FILE_OPEN_ERROR;
  }

  fclose(fhandle);
  free(all_recs);

  printf("%d row(s) updated.\n", updated);

  return rc;
}

int sem_select(token_list *t_list)
{
  int rc = 0;
  token_list *cur_token = t_list;
  tpd_entry *l_table_descriptor = NULL;
  tpd_entry *r_table_descriptor = NULL;

  char l_table_name[MAX_IDENT_LEN + 1] = {0};
  char r_table_name[MAX_IDENT_LEN + 1] = {0};
  char filename[MAX_IDENT_LEN + 5]     = {0};

  bool do_join = false;

  bool has_where       = false;
  int  where_col_index = -1;
  int  where_rel_op    = 0;   

  bool where_is_null   = false;
  bool where_is_int    = false;
  bool where_is_string = false;

  int  where_int_value = 0;
  char where_str_value[MAX_TOK_LEN] = {0};
  int  where_str_len   = 0;


  if ((cur_token == NULL) || (cur_token->tok_value != S_STAR))
  {
    rc = INVALID_STATEMENT;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }
  cur_token = cur_token->next;   

  if ((cur_token == NULL) || (cur_token->tok_value != K_FROM))
  {
    rc = INVALID_STATEMENT;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }
  cur_token = cur_token->next;   

  if ((cur_token == NULL) ||
      ((cur_token->tok_class != keyword) &&
       (cur_token->tok_class != identifier) &&
       (cur_token->tok_class != type_name)))
  {
    rc = INVALID_TABLE_NAME;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  strcpy(l_table_name, cur_token->tok_string);

  if ((l_table_descriptor = get_tpd_from_list(l_table_name)) == NULL)
  {
    rc = TABLE_NOT_EXIST;
    if (cur_token) cur_token->tok_value = INVALID;
    return rc;
  }

  cur_token = cur_token->next;   

  if ((cur_token != NULL) &&
      (cur_token->tok_class == identifier) &&
      (strcasecmp(cur_token->tok_string, "natural") == 0))
  {
    token_list *t_join  = cur_token->next;
    token_list *t_rname = (t_join ? t_join->next : NULL);

    if ((t_join == NULL) ||
        (t_join->tok_class != identifier) ||
        (strcasecmp(t_join->tok_string, "join") != 0))
    {
      rc = INVALID_STATEMENT;
      if (t_join) t_join->tok_value = INVALID;
      return rc;
    }

    if ((t_rname == NULL) ||
        ((t_rname->tok_class != keyword) &&
         (t_rname->tok_class != identifier) &&
         (t_rname->tok_class != type_name)))
    {
      rc = INVALID_TABLE_NAME;
      if (t_rname) t_rname->tok_value = INVALID;
      return rc;
    }

    strcpy(r_table_name, t_rname->tok_string);

    if ((r_table_descriptor = get_tpd_from_list(r_table_name)) == NULL)
    {
      rc = TABLE_NOT_EXIST;
      t_rname->tok_value = INVALID;
      return rc;
    }

    do_join = true;
    cur_token = t_rname->next;   
  }

  if ((cur_token != NULL) && (cur_token->tok_value == K_WHERE))
  {
    if (do_join)
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }

    has_where = true;
    cur_token = cur_token->next;   

    if ((cur_token == NULL) ||
        ((cur_token->tok_class != keyword) &&
         (cur_token->tok_class != identifier) &&
         (cur_token->tok_class != type_name)))
    {
      rc = INVALID_COLUMN_NAME;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    cd_entry *l_cols = (cd_entry*)((char*)l_table_descriptor + l_table_descriptor->cd_offset);
    where_col_index = -1;
    for (int i = 0; i < l_table_descriptor->num_columns; i++)
    {
      if (strcasecmp(l_cols[i].col_name, cur_token->tok_string) == 0)
      {
        where_col_index = i;
        break;
      }
    }
    if (where_col_index < 0)
    {
      rc = INVALID_COLUMN_NAME;
      cur_token->tok_value = INVALID;
      return rc;
    }

    cur_token = cur_token->next;   

    if ((cur_token == NULL) ||
        !((cur_token->tok_value == S_EQUAL) ||
          (cur_token->tok_value == S_GREATER) ||
          (cur_token->tok_value == S_LESS)))
    {
      rc = INVALID_STATEMENT;
      if (cur_token) cur_token->tok_value = INVALID;
      return rc;
    }

    where_rel_op = cur_token->tok_value;
    cur_token = cur_token->next;  

    if (cur_token == NULL)
    {
      rc = INVALID_STATEMENT;
      return rc;
    }

    int col_type = l_cols[where_col_index].col_type;
    int col_len  = l_cols[where_col_index].col_len;

    if (cur_token->tok_value == K_NULL)
    {
      where_is_null   = true;
      where_is_int    = false;
      where_is_string = false;

      if (where_rel_op != S_EQUAL)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }
    }
    else if (cur_token->tok_value == INT_LITERAL)
    {
      if (col_type != T_INT)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }

      where_is_int    = true;
      where_is_null   = false;
      where_is_string = false;
      where_int_value = atoi(cur_token->tok_string);
    }
    else if (cur_token->tok_value == STRING_LITERAL)
    {
      if (col_type != T_CHAR)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }

      where_is_string = true;
      where_is_null   = false;
      where_is_int    = false;

      memset(where_str_value, '\0', sizeof(where_str_value));
      strcpy(where_str_value, cur_token->tok_string);
      where_str_len = (int)strlen(where_str_value);

      if (where_str_len > col_len)
      {
        rc = INVALID_STATEMENT;
        cur_token->tok_value = INVALID;
        return rc;
      }
    }
    else
    {
      rc = INVALID_STATEMENT;
      cur_token->tok_value = INVALID;
      return rc;
    }

    cur_token = cur_token->next;   /* should be EOC */
  }

  /* EOC */
  if ((cur_token != NULL) && (cur_token->tok_value != EOC))
  {
    rc = INVALID_STATEMENT;
    cur_token->tok_value = INVALID;
    return rc;
  }

  table_file_header l_hdr;
  FILE *l_fh = NULL;

  memset(filename, '\0', sizeof(filename));
  snprintf(filename, sizeof(filename), "%s.tab", l_table_name);

  if ((l_fh = fopen(filename, "rb")) == NULL)
    return FILE_OPEN_ERROR;

  if (fread(&l_hdr, sizeof(table_file_header), 1, l_fh) != 1)
  {
    fclose(l_fh);
    return FILE_OPEN_ERROR;
  }

  l_hdr.tpd_ptr = l_table_descriptor;

  cd_entry *l_cols = (cd_entry*)((char*)l_table_descriptor + l_table_descriptor->cd_offset);
  int l_ncols = l_table_descriptor->num_columns;

  int l_col_off[MAX_NUM_COL];
  {
    int off = 0;
    for (int i = 0; i < l_ncols; i++)
    {
      l_col_off[i] = off;
      off += 1;  /* LEN */
      if (l_cols[i].col_type == T_INT)
        off += (int)sizeof(int);
      else
        off += l_cols[i].col_len;
    }
  }

   /* NATURAL JOIN  – no WHERE */
  if (do_join)
  {
    table_file_header r_hdr;
    FILE *r_fh = NULL;
    char r_filename[MAX_IDENT_LEN + 5];

    memset(r_filename, '\0', sizeof(r_filename));
    snprintf(r_filename, sizeof(r_filename), "%s.tab", r_table_name);

    if ((r_fh = fopen(r_filename, "rb")) == NULL)
    {
      fclose(l_fh);
      return FILE_OPEN_ERROR;
    }

    if (fread(&r_hdr, sizeof(table_file_header), 1, r_fh) != 1)
    {
      fclose(l_fh); fclose(r_fh);
      return FILE_OPEN_ERROR;
    }
    r_hdr.tpd_ptr = r_table_descriptor;

    cd_entry *r_cols = (cd_entry*)((char*)r_table_descriptor + r_table_descriptor->cd_offset);
    int r_ncols = r_table_descriptor->num_columns;

    int r_col_off[MAX_NUM_COL];
    {
      int off = 0;
      for (int i = 0; i < r_ncols; i++)
      {
        r_col_off[i] = off;
        off += 1;
        if (r_cols[i].col_type == T_INT)
          off += (int)sizeof(int);
        else
          off += r_cols[i].col_len;
      }
    }

    int l_join_idx[MAX_NUM_COL];
    int r_join_idx[MAX_NUM_COL];
    int J = 0;

    for (int i = 0; i < l_ncols; i++)
    {
      for (int j = 0; j < r_ncols; j++)
      {
        if (strcmp(l_cols[i].col_name, r_cols[j].col_name) == 0)
        {
          if (l_cols[i].col_type != r_cols[j].col_type)
          {
            fclose(l_fh); fclose(r_fh);
            return INVALID_STATEMENT;
          }
          if ((l_cols[i].col_type == T_CHAR) &&
              (l_cols[i].col_len  != r_cols[j].col_len))
          {
            fclose(l_fh); fclose(r_fh);
            return INVALID_STATEMENT;
          }
          l_join_idx[J] = i;
          r_join_idx[J] = j;
          J++;
        }
      }
    }

    printf("\n");
    for (int i = 0; i < l_ncols; i++)
    {
      if (i > 0) printf(" | ");
      printf("%s", l_cols[i].col_name);
    }
    for (int j = 0; j < r_ncols; j++)
    {
      bool is_common = false;
      for (int k = 0; k < J; k++)
      {
        if (r_join_idx[k] == j) { is_common = true; break; }
      }
      if (!is_common)
        printf(" | %s", r_cols[j].col_name);
    }
    printf("\n");

    int total = 0;
    for (int i = 0; i < l_ncols; i++)
    {
      if (i > 0) total += 3;
      total += (int)strlen(l_cols[i].col_name);
    }
    for (int j = 0; j < r_ncols; j++)
    {
      bool is_common = false;
      for (int k = 0; k < J; k++)
      {
        if (r_join_idx[k] == j) { is_common = true; break; }
      }
      if (!is_common)
      {
        total += 3;
        total += (int)strlen(r_cols[j].col_name);
      }
    }
    while (total-- > 0) printf("-");
    printf("\n");

    unsigned char *lrec = (unsigned char*)malloc(l_hdr.record_size);
    unsigned char *rrec = (unsigned char*)malloc(r_hdr.record_size);
    if (!lrec || !rrec)
    {
      if (lrec) free(lrec);
      if (rrec) free(rrec);
      fclose(l_fh); fclose(r_fh);
      return MEMORY_ERROR;
    }

    int printed = 0;

    for (int rl = 0; rl < l_hdr.num_records; rl++)
    {
      long lpos = l_hdr.record_offset + (long)rl * (long)l_hdr.record_size;
      if (fseek(l_fh, lpos, SEEK_SET) != 0) { rc = FILE_OPEN_ERROR; break; }
      if (fread(lrec, l_hdr.record_size, 1, l_fh) != 1) { rc = FILE_OPEN_ERROR; break; }

      for (int rr = 0; rr < r_hdr.num_records; rr++)
      {
        long rpos = r_hdr.record_offset + (long)rr * (long)r_hdr.record_size;
        if (fseek(r_fh, rpos, SEEK_SET) != 0) { rc = FILE_OPEN_ERROR; break; }
        if (fread(rrec, r_hdr.record_size, 1, r_fh) != 1) { rc = FILE_OPEN_ERROR; break; }

        bool join_match = true;
        for (int k = 0; k < J; k++)
        {
          int il = l_join_idx[k];
          int ir = r_join_idx[k];

          unsigned char *pl = lrec + l_col_off[il];
          unsigned char *pr = rrec + r_col_off[ir];
          unsigned char lenL = *pl++;
          unsigned char lenR = *pr++;

          if (lenL == 0 || lenR == 0) { join_match = false; break; }

          if (l_cols[il].col_type == T_INT)
          {
            int vL = 0, vR = 0;
            memcpy(&vL, pl, sizeof(int));
            memcpy(&vR, pr, sizeof(int));
            if (vL != vR) { join_match = false; break; }
          }
          else
          {
            if (lenL != lenR) { join_match = false; break; }
            if (memcmp(pl, pr, lenL) != 0) { join_match = false; break; }
          }
        }

        if (!join_match)
          continue;

        printed++;

        /* left columns */
        for (int c = 0; c < l_ncols; c++)
        {
          if (c > 0) printf(" | ");
          unsigned char *p = lrec + l_col_off[c];
          unsigned char len = *p++;
          if (l_cols[c].col_type == T_INT)
          {
            int v = 0; memcpy(&v, p, sizeof(int));
            if (len == 0) printf("NULL"); else printf("%d", v);
          }
          else
          {
            if (len == 0) printf("NULL");
            else
            {
              char tmp[1024]; int n = (len < (sizeof(tmp)-1)) ? len : (sizeof(tmp)-1);
              memset(tmp, 0, sizeof(tmp));
              memcpy(tmp, p, n);
              printf("%s", tmp);
            }
          }
        }

        /* right non-join columns */
        for (int c = 0; c < r_ncols; c++)
        {
          bool is_common = false;
          for (int k = 0; k < J; k++)
            if (r_join_idx[k] == c) { is_common = true; break; }
          if (is_common) continue;

          printf(" | ");
          unsigned char *p = rrec + r_col_off[c];
          unsigned char len = *p++;
          if (r_cols[c].col_type == T_INT)
          {
            int v = 0; memcpy(&v, p, sizeof(int));
            if (len == 0) printf("NULL"); else printf("%d", v);
          }
          else
          {
            if (len == 0) printf("NULL");
            else
            {
              char tmp[1024]; int n = (len < (sizeof(tmp)-1)) ? len : (sizeof(tmp)-1);
              memset(tmp, 0, sizeof(tmp));
              memcpy(tmp, p, n);
              printf("%s", tmp);
            }
          }
        }
        printf("\n");
      }
      if (rc) break;
    }

    if (!rc && (printed == 0))
      printf("(0 rows)\n");

    free(lrec); free(rrec);
    fclose(l_fh); fclose(r_fh);
    return rc;
  }


  printf("\n");
  for (int i = 0; i < l_ncols; i++)
  {
    if (i > 0) printf(" | ");
    printf("%s", l_cols[i].col_name);
  }
  printf("\n");

  int total = 0;
  for (int i = 0; i < l_ncols; i++)
  {
    if (i > 0) total += 3;
    total += (int)strlen(l_cols[i].col_name);
  }
  while (total-- > 0) printf("-");
  printf("\n");

  if (l_hdr.num_records == 0)
  {
    printf("(0 rows)\n");
    fclose(l_fh);
    return rc;
  }

  unsigned char *rec = (unsigned char*)malloc(l_hdr.record_size);
  if (!rec)
  {
    fclose(l_fh);
    return MEMORY_ERROR;
  }

  int match_count = 0;

  for (int r = 0; r < l_hdr.num_records; r++)
  {
    long pos = l_hdr.record_offset + (long)r * (long)l_hdr.record_size;
    if (fseek(l_fh, pos, SEEK_SET) != 0)
    {
      rc = FILE_OPEN_ERROR;
      break;
    }
    if (fread(rec, l_hdr.record_size, 1, l_fh) != 1)
    {
      rc = FILE_OPEN_ERROR;
      break;
    }

    bool match = false;

    if (!has_where)
    {
      match = true;
    }
    else
    {
      cd_entry *wcol = &l_cols[where_col_index];
      unsigned char *p = rec + l_col_off[where_col_index];
      unsigned char len = *p++;

      if (where_is_null)
      {
        match = (len == 0);
      }
      else if (where_is_int)
      {
        if (wcol->col_type != T_INT || len == 0)
        {
          match = false;
        }
        else
        {
          int row_val = 0;
          memcpy(&row_val, p, sizeof(int));

          if (where_rel_op == S_EQUAL)
            match = (row_val == where_int_value);
          else if (where_rel_op == S_GREATER)
            match = (row_val >  where_int_value);
          else if (where_rel_op == S_LESS)
            match = (row_val <  where_int_value);
        }
      }
      else if (where_is_string)
      {
        if (wcol->col_type != T_CHAR || len == 0)
        {
          match = false;
        }
        else
        {
          char row_str[MAX_TOK_LEN];
          int copy_len = (len < (MAX_TOK_LEN-1)) ? len : (MAX_TOK_LEN-1);
          memset(row_str, 0, sizeof(row_str));
          memcpy(row_str, p, copy_len);

          int cmp = strncmp(row_str, where_str_value,
                            (where_str_len < copy_len ? where_str_len : copy_len));
          if (cmp == 0)
          {
            if (copy_len == where_str_len)
              cmp = 0;
            else if (copy_len > where_str_len)
              cmp = 1;
            else
              cmp = -1;
          }

          if (where_rel_op == S_EQUAL)
            match = (cmp == 0);
          else if (where_rel_op == S_GREATER)
            match = (cmp > 0);
          else if (where_rel_op == S_LESS)
            match = (cmp < 0);
        }
      }
    }

    if (!match)
      continue;

    match_count++;

    for (int c = 0; c < l_ncols; c++)
    {
      if (c > 0) printf(" | ");
      unsigned char *p = rec + l_col_off[c];
      unsigned char len = *p++;
      if (l_cols[c].col_type == T_INT)
      {
        int v = 0; memcpy(&v, p, sizeof(int));
        if (len == 0) printf("NULL"); else printf("%d", v);
      }
      else
      {
        if (len == 0) printf("NULL");
        else
        {
          char tmp[1024]; int n = (len < (sizeof(tmp)-1)) ? len : (sizeof(tmp)-1);
          memset(tmp, 0, sizeof(tmp));
          memcpy(tmp, p, n);
          printf("%s", tmp);
        }
      }
    }
    printf("\n");
  }

  if ((rc == 0) && (match_count == 0))
    printf("Warning: no rows selected.\n");

  free(rec);
  fclose(l_fh);

  return rc;
}


int initialize_tpd_list() {
  int rc = 0;
  FILE *fhandle = NULL;
  struct stat file_stat;

  if ((fhandle = fopen("dbfile.bin", "rbc")) == NULL) {
    if ((fhandle = fopen("dbfile.bin", "wbc")) == NULL) {
      rc = FILE_OPEN_ERROR;
    } else {
      g_tpd_list = NULL;
      g_tpd_list = (tpd_list *)calloc(1, sizeof(tpd_list));

      if (!g_tpd_list) {
        rc = MEMORY_ERROR;
      } else {
        g_tpd_list->list_size = sizeof(tpd_list);
        fwrite(g_tpd_list, sizeof(tpd_list), 1, fhandle);
        fflush(fhandle);
        fclose(fhandle);
      }
    }
  } else {
    fstat(fileno(fhandle), &file_stat);
    printf("dbfile.bin size = %d\n", file_stat.st_size);

    g_tpd_list = (tpd_list *)calloc(1, file_stat.st_size);

    if (!g_tpd_list) {
      rc = MEMORY_ERROR;
    } else {
      fread(g_tpd_list, file_stat.st_size, 1, fhandle);
      fflush(fhandle);
      fclose(fhandle);

      if (g_tpd_list->list_size != file_stat.st_size) {
        rc = DBFILE_CORRUPTION;
      }
    }
  }

  return rc;
}

int add_tpd_to_list(tpd_entry *tpd) {
  int rc = 0;
  int old_size = 0;
  FILE *fhandle = NULL;

  if ((fhandle = fopen("dbfile.bin", "wbc")) == NULL) {
    rc = FILE_OPEN_ERROR;
  } else {
    old_size = g_tpd_list->list_size;

    if (g_tpd_list->num_tables == 0) {
      /* If this is an empty list, overlap the dummy header */
      g_tpd_list->num_tables++;
      g_tpd_list->list_size += (tpd->tpd_size - sizeof(tpd_entry));
      fwrite(g_tpd_list, old_size - sizeof(tpd_entry), 1, fhandle);
    } else {
      /* There is at least 1, just append at the end */
      g_tpd_list->num_tables++;
      g_tpd_list->list_size += tpd->tpd_size;
      fwrite(g_tpd_list, old_size, 1, fhandle);
    }

    fwrite(tpd, tpd->tpd_size, 1, fhandle);
    fflush(fhandle);
    fclose(fhandle);
  }

  return rc;
}

int drop_tpd_from_list(char *tabname) {
  int rc = 0;
  tpd_entry *cur = &(g_tpd_list->tpd_start);
  int num_tables = g_tpd_list->num_tables;
  bool found = false;
  int count = 0;

  if (num_tables > 0) {
    while ((!found) && (num_tables-- > 0)) {
      if (strcasecmp(cur->table_name, tabname) == 0) {
        /* found it */
        found = true;
        int old_size = 0;
        FILE *fhandle = NULL;

        if ((fhandle = fopen("dbfile.bin", "wbc")) == NULL) {
          rc = FILE_OPEN_ERROR;
        } else {
          old_size = g_tpd_list->list_size;

          if (count == 0) {
            /* If this is the first entry */
            g_tpd_list->num_tables--;

            if (g_tpd_list->num_tables == 0) {
              /* This is the last table, null out dummy header */
              memset((void *)g_tpd_list, '\0', sizeof(tpd_list));
              g_tpd_list->list_size = sizeof(tpd_list);
              fwrite(g_tpd_list, sizeof(tpd_list), 1, fhandle);
            } else {
              /* First in list, but not the last one */
              g_tpd_list->list_size -= cur->tpd_size;

              /* First, write the 8 byte header */
              fwrite(g_tpd_list, sizeof(tpd_list) - sizeof(tpd_entry), 1,
                     fhandle);

              /* Now write everything starting after the cur entry */
              fwrite((char *)cur + cur->tpd_size,
                     old_size - cur->tpd_size -
                         (sizeof(tpd_list) - sizeof(tpd_entry)),
                     1, fhandle);
            }
          } else {
            /* This is NOT the first entry - count > 0 */
            g_tpd_list->num_tables--;
            g_tpd_list->list_size -= cur->tpd_size;

            /* First, write everything from beginning to cur */
            fwrite(g_tpd_list, ((char *)cur - (char *)g_tpd_list), 1, fhandle);

            /* Check if cur is the last entry. Note that g_tdp_list->list_size
               has already subtracted the cur->tpd_size, therefore it will
               point to the start of cur if cur was the last entry */
            if ((char *)g_tpd_list + g_tpd_list->list_size == (char *)cur) {
              /* If true, nothing else to write */
            } else {
              /* NOT the last entry, copy everything from the beginning of the
                 next entry which is (cur + cur->tpd_size) and the remaining
                 size */
              fwrite((char *)cur + cur->tpd_size,
                     old_size - cur->tpd_size -
                         ((char *)cur - (char *)g_tpd_list),
                     1, fhandle);
            }
          }

          fflush(fhandle);
          fclose(fhandle);
        }
      } else {
        if (num_tables > 0) {
          cur = (tpd_entry *)((char *)cur + cur->tpd_size);
          count++;
        }
      }
    }
  }

  if (!found) {
    rc = INVALID_TABLE_NAME;
  }

  return rc;
}

tpd_entry *get_tpd_from_list(char *tabname) {
  tpd_entry *tpd = NULL;
  tpd_entry *cur = &(g_tpd_list->tpd_start);
  int num_tables = g_tpd_list->num_tables;
  bool found = false;

  if (num_tables > 0) {
    while ((!found) && (num_tables-- > 0)) {
      if (strcasecmp(cur->table_name, tabname) == 0) {
        /* found it */
        found = true;
        tpd = cur;
      } else {
        if (num_tables > 0) {
          cur = (tpd_entry *)((char *)cur + cur->tpd_size);
        }
      }
    }
  }

  return tpd;
}
