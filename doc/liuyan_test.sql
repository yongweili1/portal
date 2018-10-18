use ai_portal;
drop table if exists graph_element; 
create table graph_element
(
   graphelementuid      int auto_increment,
   imageuid 		    varchar(64) not null,
   overlaytype          varchar(64),
   dotproductpath       varchar(255),
   importdatatime		datetime,
   updatetime           timestamp not null default CURRENT_TIMESTAMP ON UPDATE CURRENT_TIMESTAMP,

   primary key (graphelementuid),
   unique key (graphelementuid),
   foreign key (imageuid) references image(imageuid) on delete cascade
)
ENGINE = InnoDB
DEFAULT CHARACTER SET = utf8;