/* 
Xiaoqing Shangguan Created On 2018.06.14
Initial Version, includes 4 parts: DCM related, user realted, sysconfig related, ai application related
*/
drop schema IF EXISTS ai_portal;
create schema IF NOT EXISTS ai_portal;
use ai_portal;

/* DCM related */
drop table if exists patient;
drop table if exists study;
drop table if exists series;
drop table if exists image;
drop table if exists graph_element; 
/* user related */
drop table if exists role;
drop table if exists user;
drop table if exists user_role;
/* sysconfig realted*/
drop table if exists dbversion;
drop table if exists sysconfig;
/* sys health realted*/
drop table if exists workflow_service_status;
drop table if exists ai_process_log;
/* application related, general lookup table for AI process status*/
drop table if exists series_process_status;
/* application related, naming standard AppName+ResultCategory*/
drop table if exists lung_segment;
/* CSV related */
drop table if exists alg_csv;
/* script related */
drop table if exists script;

create table patient
(
   pid					int auto_increment,
   patientid            varchar(64) not null default '' comment '(0010,0020)',
   patientname          varchar(64) comment '(0010,0010)',
   patientbirthdate     date comment '(0010,0030)',
   patientsex           varchar(16) comment '(0010,0040)',
   patientsize          varchar(16) default '' comment '(0010,1020) ',
   patientweight        varchar(16) default '' comment '(0010,1030)',  
   patientcomments      varchar(1024) default '' comment '(0010,4000) ',   

   importdatetime       datetime comment '病人导入数据库的时间',   
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

   primary key (pid),
   unique key (patientid)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table study
(
   pid					int auto_increment,
   studyuid             varchar(64) not null default '' comment '(0020,000D) ',
   patientid            varchar(64) not null default '' comment 'FK to patient',
   studyid              varchar(16) default '' comment '(0020,0010) ',
   studydate            date comment '(0008,0020)',
   studytime            time comment '(0008,0030)',
   studydescription     varchar(1024) default '' comment '(0008,1030)',

   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP ,   
   primary key (pid),
   unique key (studyuid),
   foreign key (patientid) references patient(patientid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table series
(
   pid					int auto_increment,
   seriesuid            varchar(64) not null default '' comment '(0020,000E)',
   buildvolumesign      int default 1,
   studyuid             varchar(64) default '' comment 'Fk to study',
   
   modality             varchar(16) not null default '' comment '(0008,0060) ',
   seriesnumber         int default 0 comment '(0020,0011) ',
   seriesdate           date comment '(0008,0021) ',
   seriestime           time comment '(0008,0031) ',
   seriesdescription    varchar(64) default '' comment '(0008,103E) ',

   bodypartexamined     varchar(16) default '' comment '(0018,0015) ',
   patientposition      varchar(16) default '' comment '(0018,5100) ',
   manufacturer         varchar(64) default '' comment '(0008,0070)',
   institutionname      varchar(64) default '' comment '(0008,0080) ',

   seriespixeldatafilepath varchar(255) comment '存放Image3D文件',

   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP ,
   primary key (pid),
   unique key (seriesuid),
   foreign key (studyuid) references study(studyuid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table image
(
   pid					int auto_increment,
   imageuid             varchar(64) not null default '' comment '(0008,0018) ',
   updatesign           int default 0,
   seriesuid            varchar(64) not null comment 'Fk to series', 
   instancenumber       int default 0 comment '(0020,0013) ',
   patientorientation   varchar(33) default '' comment '(0020,0020)',
   acquisitiondatetime  datetime comment '(0008,002A) ',   
   rowcount             int default 0 comment '(0028,0010) ',
   columncount          int default 0 comment '(0028,0011)',
   bitsallocated        int default 0 comment '(0028,0100) ',
   bitsstored           int comment '(0028,0101)',
   highbit              int comment '(0028,0102)',
   windowwidth          varchar(135) comment '(0028,1051)',
   windowcenter         varchar(135) comment '(0028,1050) ',
   imagecomments        varchar(10240) default '' comment '(0020,4000) ',
   lossyimagecompression varchar(16) default '' comment '(0028,2110)',
   lossyimagecompressionratio varchar(271) default '' comment '(0028,2112)',
   pixelspacing         varchar(64) default '' comment '(0028,0030) ',
   imageorientationpatient varchar(101) default '' comment '(0020,0037)',
   imagepositionpatient varchar(50) default '' comment '(0020,0032)',
   slicethickness       varchar(16) default '' comment '(0018,0050) ',
   slicelocation        varchar(16) default '' comment '(0020,1041) ',
   samplesperpixel      int default 0 comment '(0028,0002) ',
   rescaleintercept     varchar(16) comment '(0028,1052)',
   rescaleslope         varchar(16) comment '(0028,1053)',
   rescaletype          varchar(16) comment '(0028,1054)',
   
   dcmfilepath          varchar(255) comment 'For dcm filepath',
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

   primary key (pid),
   unique key (imageuid),
   foreign key (seriesuid) references series(seriesuid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table graph_element
(
   graphelementuid      int auto_increment,
   imageuid 		    varchar(64) not null,
   dotproductpath       varchar(255),
   importdatatime		datetime,
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

   primary key (graphelementuid),
   unique key (graphelementuid),
   foreign key (imageuid) references image(imageuid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table script
(
   pid                  int auto_increment,
   scriptname           varchar(64) not null,
   userid               varchar(64) not null,
   scriptpath           varchar(255),
   importdatatime		datetime,
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
		
   primary key (pid),
   unique key (scriptname)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table new_django_session
(
   client_ip            varchar(64) not null,
   session_key          varchar(40) not null,
   session_data         varchar(255) not null,
   expire_date          datetime,

   primary key (session_key),
   unique key (session_key)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;
/*
create table role
(
   uid                  varchar(64) not null,
   rolename             varchar(64) ,
   description          varchar(1024) ,
   systemrole           int comment 'doctor, admin, other',
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (uid)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table user
(
   id                   varchar(64) not null ,
   gender               int ,
   password             varchar(1024) not null ,
   description          varchar(1024) ,
   phone                varchar(32) ,
   isenabled            bool not null ,
   firstname            varchar(64) ,
   lastname             varchar(64) ,

   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (id)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table user_role
(
   userid               varchar(64) not null ,
   roleuid              varchar(64) not null ,
   
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,
   primary key (roleuid, userid)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table sysconfig
(
   name                varchar(128) not null,
   value               varchar(10000) ,  
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP ,
   
   primary key (name)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table dbversion
(
   version              varchar(64) not null comment '数据库版本号',
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP ,
   
   primary key (version)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table workflow_service_status
(
   pid					int auto_increment,
   processid            int not null,
   createdtime			timestamp not null,
   updatetime           timestamp not null,
   
   primary key (pid)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table ai_process_log
(
   pid					int auto_increment,
   processid            int not null,
   createdtime			timestamp not null,
   status               int not null default 0 comment '0-new created, 1-time out kill, 2-scheduled kill', 
   
   primary key (pid)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;

create table series_process_status
(
   pid					int auto_increment,
   seriesuid            varchar(64) not null default '' comment '(0020,000E)',
   issegprocessed       bool not null ,
   islungprocessed      bool not null ,
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP ,
   primary key (pid),
   unique key (seriesuid),
   foreign key (seriesuid) references series(seriesuid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;
*/

create table alg_csv
(
   pid					int auto_increment,
   image_path           varchar(64) not null default '' ,
   x                    float not null ,
   y                    float not null ,
   z					int not null,
   width                float not null,
   height               float not null,
   depth                int not null,
   probability          int not null,
   classname            varchar(64) not null default '' ,
   location             varchar(64) not null default '' ,

   primary key (pid),
   unique key (image_path)
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;