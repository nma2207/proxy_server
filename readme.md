commands:
start psql:
```sudo systemctl start postgres```

connect to server:
```psql -U postgres -p 8080 -h localhost testdb --set=sslmode=disable```

pgbench:
```
pgbench --user  postgres  --host localhost --port 8080 --client 100  testdb            2 ✘ 
pgbench (14.5)
starting vacuum...end.
transaction type: <builtin: TPC-B (sort of)>
scaling factor: 1
query mode: simple
number of clients: 100
number of threads: 1
number of transactions per client: 10
number of transactions actually processed: 1000/1000
latency average = 134.092 ms
initial connection time = 273.993 ms
tps = 745.756645 (without initial connection time)
```

