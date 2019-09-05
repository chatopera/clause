/***************************************************************************
 *
 * Copyright (c) 2019 Chatopera.Inc, Inc. All Rights Reserved
 *
 **************************************************************************/

namespace cpp chatopera.bot.sysdicts

typedef string Timestamp

struct Entity {
    1: optional string slotname;
    2: optional string dictname;
    3: optional string val;
    4: optional double score;
}

struct Data {
    1: optional i32 rc;
    2: optional string msg;
    3: optional string error;
    4: optional string query;
    5: optional list<Entity> entities;
}

/**
 * API
 */
service Serving {
    Data label(1: Data request);
}