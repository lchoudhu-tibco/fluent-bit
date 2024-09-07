/* -*- Mode: C; tab-width: 4; indent-tabs-mode: nil; c-basic-offset: 4 -*- */

/*  Fluent Bit
 *  ==========
 *  Copyright (C) 2015-2024 The Fluent Bit Authors
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

#ifndef OUT_AZURE_BLOB_DB_H
#define OUT_AZURE_BLOB_DB_H

#include <fluent-bit/flb_output_plugin.h>
#include "azure_blob.h"

#define SQL_PRAGMA_FOREIGN_KEYS "PRAGMA foreign_keys = ON;"

#define SQL_CREATE_AZURE_BLOB_FILES                                       \
    "CREATE TABLE IF NOT EXISTS out_azure_blob_files ("                   \
    "  id        INTEGER PRIMARY KEY,"                                    \
    "  path      TEXT NOT NULL,"                                          \
    "  size      INTEGER,"                                                \
    "  created   INTEGER"                                                 \
    ");"

#define SQL_CREATE_AZURE_BLOB_PARTS                                       \
    "CREATE TABLE IF NOT EXISTS out_azure_blob_parts ("                   \
    "  id           INTEGER PRIMARY KEY,"                                 \
    "  file_id      INTEGER NOT NULL,"                                    \
    "  uploaded     INTEGER DEFAULT 0,"                                   \
    "  offset_start INTEGER,"                                             \
    "  offset_end   INTEGER,"                                             \
    "  FOREIGN KEY (file_id) REFERENCES out_azure_blob_files(id) "        \
    "    ON DELETE CASCADE"                                               \
    ");"

#define SQL_INSERT_BLOB_FILE                                              \
    "INSERT INTO out_azure_blob_files (path, size, created)"              \
    "  VALUES (@path, @size, @created);"

/* DELETE a registered file and all it parts */
#define SQL_DELETE_BLOB_FILE                                              \
    "DELETE FROM out_azure_blob_files WHERE id=@id;"

#define SQL_GET_BLOB_FILE                                                 \
    "SELECT * from out_azure_blob_files WHERE path=@path order by id desc;"

#define SQL_INSERT_BLOB_FILE_PART                                          \
    "INSERT INTO out_azure_blob_parts (file_id, offset_start, offset_end)" \
    "  VALUES (@file_id, @offset_start, @offset_end);"

#define SQL_UPDATE_BLOB_PART_UPLOADED                                      \
    "UPDATE out_azure_blob_parts SET uploaded=1 WHERE id=@id;"

struct flb_sqldb *azb_db_open(struct flb_azure_blob *ctx, char *db_path);
int azb_db_close(struct flb_sqldb *db);
int azb_db_file_exists(struct flb_azure_blob *ctx, char *path, uint64_t *id);
int64_t azb_db_file_insert(struct flb_azure_blob *ctx, char *path, size_t size);
int azb_db_file_delete(struct flb_azure_blob *ctx, uint64_t id, char *path);

int azb_db_file_part_insert(struct flb_azure_blob *ctx, uint64_t file_id,
                            size_t offset_start, size_t offset_end,
                            int64_t *out_id);

#endif