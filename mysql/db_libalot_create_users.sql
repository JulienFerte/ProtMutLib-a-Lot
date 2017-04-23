use DB_LIBALOT;

create user 'LibALotEngine'@'%' identified by 'other_password';

grant all privileges on * to 'LibALotEngine'@'%';

create user 'LibALotUser'@'%' identified by 'password';

grant select on `TB_CHAIN`              to 'LibALotUser'@'%';
grant select on `TB_CHAIN_SEQUENCE`     to 'LibALotUser'@'%';
grant select on `TB_MODULE1`            to 'LibALotUser'@'%';
grant select on `TB_MODULE2`            to 'LibALotUser'@'%';
grant select on `TB_JOB`                to 'LibALotUser'@'%';
grant select on `TB_MUTATION`           to 'LibALotUser'@'%';
grant select on `TB_POINT_MUTATION`     to 'LibALotUser'@'%';
grant select on `TB_PROTEIN`            to 'LibALotUser'@'%';
grant select on `TB_SEQUENCE`           to 'LibALotUser'@'%';
grant select on `TB_SEQUENCE_STRUCTURE` to 'LibALotUser'@'%';
grant select on `TB_STRUCTURE`          to 'LibALotUser'@'%';
grant select on `VW_DISTRIBUTION`       to 'LibALotUser'@'%';
grant select on `VW_DISTRIBUTION_AUX`   to 'LibALotUser'@'%';
grant select on `VW_JOB`                to 'LibALotUser'@'%';
grant select on `VW_MUTATION`           to 'LibALotUser'@'%';
grant select on `VW_PROTEIN_SEQUENCE`   to 'LibALotUser'@'%';

flush privileges;
