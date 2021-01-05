# Public Cloud 3사 DB 비교
    

## 1. 비교


* #### 1.1 DB PaaS 비교
|               |AWS   |          |Azure                   |                          |GCP          |        |
|---------------|------|----------|------------------------|--------------------------|-------------|--------|
|**관계형 DB**  |Aurora|MySQL     |Azure SQL               |Azure SQL Database        |Cloud Spanner|
|               |      |PostgreSQL|                        |Azure SQL Managed Instance|             |
|               |RDS   |MySQL     |Azure Database          |Database for MySQL        |Cloud SQL    |MySQL   |
|               |      |PostgreSQL||Database for PostgreSQL|                          |PostgreSQL   |
|               |      |MariaDB   ||Database for MariaDB   |                          |SQL Server   |
|               |      |Oracle    |
|               |      |SQL Server|
|**MPP DW**     |      |RedShift  |                        |Azure Synapse Analytics   |             |BigQuery|
|**비관계형 DB**|      |DynamoDB  |                        |Cosmos DB|                |             |        |Filestore|
|               |      |Redis     |                        |Cache for Redis           |             |        |Memorystore|
|               |      |DocumentDB|                        |                          |             |        |Cloud Bigtable|