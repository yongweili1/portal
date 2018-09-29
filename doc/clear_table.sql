use ai_portal;

DELETE FROM `ai_portal`.`patient` where pid != 0;
DELETE FROM `ai_portal`.`study` where pid != 0 ;
DELETE FROM `ai_portal`.`series` where pid != 0;
DELETE FROM `ai_portal`.`image` where pid != 0;