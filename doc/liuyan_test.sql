use ai_portal;
drop table if exists roi; 
create table roi
(
	pid                 int auto_increment,
	roiname             varchar(64) not null,
    seriesuid           varchar(64) not null,
	roicolor            varchar(64) not null,
    importdatatime		datetime,
    updatetime          timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

	primary key (pid),
    unique key (pid),
    foreign key (seriesuid) references series(seriesuid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;