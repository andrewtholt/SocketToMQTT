--
-- Sqlite 3
--

drop table if exists map;

create table map (
    idx INTEGER PRIMARY KEY,
    client_name varchar(32) NOT NULL DEFAULT 'TESTING',
    name       varchar(32) not null,
    cmdTopic   varchar(32) ,
    stateTopic varchar(32) NOT NULL DEFAULT 'NONE' ,
    value      varchar(32) default '<UNKNOWN>',
    direction  TEXT CHECK(direction IN ('PUB','SUB','LOCAL')) NOT NULL DEFAULT 'LOCAL',
    --
    -- Time To Live, in seconds
    --
    TTL int default 0 ,
    dirty int default 0,
    logtime int
);

CREATE TRIGGER UpdateLastTime
AFTER UPDATE ON map FOR EACH ROW
BEGIN
    UPDATE map SET logtime = CURRENT_TIMESTAMP,  dirty=1 WHERE rowid = new.rowid;
END ;

insert into map ('client_name','name','cmdTopic','direction') values ('TEST','start','/test/start/cmnd','PUB');
insert into map ('client_name','name','cmdTopic','direction') values ('TEST','stop','/test/stop/cmnd','PUB');

insert into map ('client_name','name','cmdTopic','stateTopic','direction')
    values ('TEST','motor','/test/motor/cmnd', '/test/motor/POWER', 'SUB');

