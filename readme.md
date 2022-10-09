commands:
start psql:
```sudo systemctl start postgres```

connect to server:
```psql -U postgres -p 8080 -h localhost testdb --set=sslmode=disable```

sysbench:

```
sysbench /usr/share/sysbench/tests/include/oltp_legacy/parallel_prepare.lua 
--db-driver=pgsql 
--oltp-table-size=100
--oltp-tables-count=100
--threads=10
 --pgsql-host=localhost 
 --pgsql-port=8080
 --pgsql-user=postgres
 --pgsql-password=postgres
 --pgsql-db=testdb run
```
