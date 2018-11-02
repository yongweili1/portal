use ai_portal;
drop table if exists contours; 
create table contours
(
   uid      			int auto_increment,
   imageuid 		    varchar(64) not null,
   dotsetpath	        varchar(255),
   organ				varchar(255),
   instance_no          int,
   importdatatime		datetime,
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

   primary key (uid),
   unique key (uid),
   foreign key (imageuid) references image(imageuid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;