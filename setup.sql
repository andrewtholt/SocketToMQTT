-- 
-- Sqlite 3
--

drop table if exists map;

create table map (
    idx INTEGER PRIMARY KEY,
    name       varchar(32) not null,
    cmdTopic   varchar(32) ,
    stateTopic varchar(32) ,
    value      varchar(32) default '<UNKNOWN>',
    -- 
    -- Time To Live, in seconds
    -- 
    TTL int default 0 ,
    logtime int
);

CREATE TRIGGER UpdateLastTime
AFTER UPDATE ON map FOR EACH ROW
BEGIN
    UPDATE map SET logtime = CURRENT_TIMESTAMP WHERE rowid = new.rowid;
END ;

insert into map ('name','cmdTopic') values ('start','/test/start/cmnd');
insert into map ('name','cmdTopic') values ('stop','/test/stop/cmnd');
insert into map ('name','cmdTopic') values ('motor','/test/motor/cmnd');

