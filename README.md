# Tibero 조사 쿼리(Simple)

Chem의 티베로의 조사를 하는 쿼리   

## 1. Tibero ASIS조사 쿼리
ASIS Tibero 5.0 기준

* #### 1.1 인스턴스 조사
    ```sql
    SELECT * FROM V$INSTANCE;
    ```

* #### 1.2 캐릭터셋 조사
    ```sql
    SELECT * FROM DATABASE_PROPERTIES WHERE NAME IN ('NLS_CHARACTERSET','NLS_NCHAR_CHARACTERSET');
    ```

* #### 1.3 국가 / 언어
    ```sql
    SELECT * FROM V$PARAMETERS WHERE NAME IN ( 'NLS_DATE_LANGUAGE','NLS_DATE_FORMAT');
    ```

* #### 1.4 현재 세션 리스트 
    ```sql
    SELECT username,TYPE, MACHINE, OSUSER,PROG_NAME, COUNT(1)
    FROM v$session
    GROUP BY username,TYPE, MACHINE, OSUSER,PROG_NAME;
    ```

* #### 1.5 소유자별 오브젝트 갯수 
    ```sql
    SELECT owner, object_type, count(*) CNT
    FROM dba_objects 
    WHERE owner  not in ('SYS','SYSCAT','SYSGIS','PUBLIC','OUTLN','TIBERO','WMSYS')
    GROUP BY owner, object_type
    ORDER BY owner,object_type;
    ```

* #### 1.6 INVALID 찾기
    ```sql
    SELECT OWNER, OBJECT_NAME, OBJECT_TYPE,  STATUS
    FROM DBA_OBJECTS
    WHERE OWNER NOT IN ('SYS','SYSCAT','SYSGIS','OUTLN','WMSYS','TIBERO','TIBERO1','PUBLIC')
        AND STATUS < > 'VALID';
    ```

* #### 1.7 계정 상태
    ```sql
    SELECT USERNAME,DEFAULT_TABLESPACE,DEFAULT_TEMP_TABLESPACE,PROFILE,ACCOUNT_STATUS,CREATED
    FROM DBA_USERS A
    ORDER BY DECODE(USERNAME,'SYS',2,'SYSCAT',2,'SYSGIS',2,'OUTLN',2,'WMSYS',2,'TIBERO',2,'TIBERO1',2,1);
    ```

* #### 1.8 계정별 용량
    ```sql
    SELECT owner , sum(bytes/1024/1024) SizeMB
    FROM dba_segments
    GROUP BY owner ;
    ```

* #### 1.9 테이블별 용량
    ```sql
    SELECT OWNER, TABLE_NAME, TABLESPACE_NAME
        ,SUM(NOMAL_COL)/1024/1024 NOMAL_COL_SIZE_MB
        ,SUM(LOB_COL)/1024/1024 LOB_COL_SIZE_MB
    FROM
    (
        SELECT A.OWNER, DECODE(A.SEGMENT_TYPE,'LOB',B.TABLE_NAME,A.SEGMENT_NAME) TABLE_NAME, A.SEGMENT_TYPE
            , A.TABLESPACE_NAME
            , DECODE(A.SEGMENT_TYPE,'LOB',0,A.BYTES) NOMAL_COL
            , DECODE(A.SEGMENT_TYPE,'LOB',A.BYTES,0) LOB_COL
        FROM DBA_SEGMENTS A
                , DBA_LOBS B 
        WHERE A.OWNER NOT IN ('SYS','SYSCAT','SYSGIS','OUTLN','WMSYS','TIBERO','TIBERO1')
        AND A.SEGMENT_TYPE IN ('TABLE','LOB')
        AND A.OWNER = B.OWNER(+)
        AND A.SEGMENT_NAME = B.SEGMENT_NAME(+)
    )
    GROUP BY OWNER,TABLE_NAME,TABLESPACE_NAME;
    ```


## 2. Tibero Import / Export Util 사용법

* #### 2.1 tbexport
    데이터 로딩시 권장사항  
    
    ```
    tbexport sid=mydb username=myuser password=a!3gakld File=C:\_LG화학\EXPORT\TEST.dat script=y IP=192.168.0.1 USER=myapple GRANT=N
    ```

    옵션 설명
    * a. File   
        export 받을 대상 파일

    * b. script   
        스크립트를 같이 받을지 여부 default는 N

    * c. IP   
        Export 대상 Tibero 서버의 IP 주소를 입력한다. (기본값: localhost)

    * d. USER   
        Export를 수행할 때 권한의 Export 여부를 지정 (기본값: Y)

    * e. GRANT   
        사용자 모드로 Export를 수행할 때 Export될 객체의 소유자를 지정한다.
        이거 지정하면 해당 유저의 모든 Object 대상

    * f. ROWS   
        export를 수행할 때 테이블의 데이터를 Export할지 여부를 지정 (기본값: Y)
        테이블생성 스크립트만 할때는 N을 지정



* #### 2.2 tbimport
    ```    
    tbimport IP=192.168.0.1 sid=mydb USER=myuser username=myuser password=abdk@dj2 File=E:\dbadm\Sungchool\CERT_USER_20200924.dat COMMIT=Y
    ```

    * a. IP        
        Export 대상 Tibero 서버의 IP 주소를 입력한다. (기본값: localhost)

    * b. COMMIT        
        insert 작업 후에 commit을 수행한다. (기본값: N)        
            - CPL로 import할 때 기본적으로 bind insert buffer size인 1MB를 넘었을 때 commit을 수행   
            - DPL로 import할 때 BIND_BUF_SIZE로 지정된 크기를 넘었을 때 commit을 수행한다.
	
    * c. DPL   
        DPL(Direct Path Load) 방법으로 Import할지 여부를 지정. (기본: N)

    * d. BIND_BUF_SIZE   
        Import를 DPL 모드로 실행할 때 stream에서 사용하는 bind buffer의 크기. (기본값: 1MB(1048576))

    * e. File   
        export 받을 대상 파일


# Oracle 조사 쿼리(Simple)
LGChem의 Oracle 정보 조회 쿼리

## 1. Oracle ASIS조사 쿼리
ASIS Oracle 10.2.0.3 기준

* #### 1.1 인스턴스 조사
    ```sql
    SELECT * FROM V$INSTANCE;
    ```

* #### 1.2 DB 용량 확인
    ```sql
    SELECT sum(bytes)/1024/1024/1024 FROM dba_segments;
    ```

* #### 1.3 캐릭터셋 조사
    ```sql
    SELECT * FROM NLS_DATABASE_PARAMETERS;
    ```

* #### 1.4 국가 / 언어
    ```sql
    SELECT * FROM NLS_DATABASE_PARAMETERS;
    ```

* #### 1.5 Redo Log 일별 발생량
    ```sql
    SELECT
    SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH:MI:SS'),1,5) AS DAY
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'00',1,0)) AS H00
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'01',1,0)) AS H01
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'02',1,0)) AS H02
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'03',1,0)) AS H03
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'04',1,0)) AS H04
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'05',1,0)) AS H05
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'06',1,0)) AS H06
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'07',1,0)) AS H07
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'08',1,0)) AS H08
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'09',1,0)) AS H09
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'10',1,0)) AS H10
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'11',1,0)) AS H11
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'12',1,0)) AS H12
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'13',1,0)) AS H13
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'14',1,0)) AS H14
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'15',1,0)) AS H15
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'16',1,0)) AS H16
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'17',1,0)) AS H17
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'18',1,0)) AS H18
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'19',1,0)) AS H19
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'20',1,0)) AS H20
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'21',1,0)) AS H21
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'22',1,0)) AS H22
    ,SUM(DECODE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH24:MI:SS'),10,2),'23',1,0)) AS H23
    ,COUNT(*) AS TOTAL
    FROM v$log_history a
        WHERE (TO_DATE(SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH:MI:SS'), 1,8), 'MM/DD/RR') >= to_char(sysdate-18,'YYYYMMDD'))
        AND (TO_DATE(substr(TO_CHAR(first_time, 'MM/DD/RR HH:MI:SS'), 1,8), 'MM/DD/RR') <= to_char(sysdate-1,'YYYYMMDD'))
        GROUP BY SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH:MI:SS'),1,5)
        ORDER BY SUBSTR(TO_CHAR(first_time, 'MM/DD/RR HH:MI:SS'),1,5) DESC;
    ```

* #### 1.6 현재 세션 리스트 
    ```sql
    SELECT username,TYPE, MACHINE, OSUSER, PROGRAM, COUNT(*)
    FROM v$session
    WHERE TYPE NOT IN ('BACKGROUND') 
    GROUP BY username,TYPE, MACHINE, OSUSER, PROGRAM;
    ```

* #### 1.7 계정별 오브젝트 갯수 
    ```sql
    SELECT owner, object_type, count(*) CNT
    FROM dba_objects 
    WHERE owner NOT IN ('OUTLN','MGMT_VIEW','SYS','SYSTEM','EXFSYS','WKSYS','XDB','SI_INFORMTN_SCHEMA','CTXSYS','ANONYMOUS','DMSYS','WK_TEST','WMSYS','ORDSYS','ORDPLUGINS','WKPROXY','MDSYS','SYSMAN','DBSNMP','TSMSYS','DIP','MDDATA','DEMO','AURORA$ORB$UNAUTHENTICATED','AWR_STAGE','CSMIG','DSSYS','LBACSYS','ORACLE_OCM','PERFSTAT','TRACESVR','OLAPSYS')
    GROUP BY owner, object_type
    ORDER BY owner,object_type;
    ```

* #### 1.8 계정 상태
    ```sql
    SELECT USERNAME,DEFAULT_TABLESPACE,TEMPORARY_TABLESPACE,PROFILE,ACCOUNT_STATUS,CREATED
    FROM DBA_USERS; 
    ```

* #### 1.9 계정별 세그먼트 용량 
    ```sql
    SELECT owner, sum(bytes)/1024/1024 
    FROM dba_segments
	GROUP BY owner;
    ```

* #### 1.10 INVALID 찾기
    ```sql
    SELECT OWNER, OBJECT_NAME, OBJECT_TYPE,  STATUS
    FROM DBA_OBJECTS
    WHERE owner NOT IN ('OUTLN','MGMT_VIEW','SYS','SYSTEM','EXFSYS','WKSYS','XDB','SI_INFORMTN_SCHEMA','CTXSYS','ANONYMOUS','DMSYS','WK_TEST','WMSYS','ORDSYS','ORDPLUGINS','WKPROXY','MDSYS','SYSMAN','DBSNMP','TSMSYS','DIP','MDDATA','DEMO','AURORA$ORB$UNAUTHENTICATED','AWR_STAGE','CSMIG','DSSYS','LBACSYS','ORACLE_OCM','PERFSTAT','TRACESVR','OLAPSYS')
    AND STATUS = 'INVALID';
    ```

* #### 1.11 테이블별 용량
    ```sql
    SELECT OWNER, TABLE_NAME, TABLESPACE_NAME
        ,SUM(NOMAL_COL)/1024/1024 NOMAL_COL_SIZE_MB
        ,SUM(LOB_COL)/1024/1024 LOB_COL_SIZE_MB
    FROM
    (
        SELECT A.OWNER, DECODE(A.SEGMENT_TYPE,'LOB',B.TABLE_NAME,A.SEGMENT_NAME) TABLE_NAME, A.SEGMENT_TYPE
            , A.TABLESPACE_NAME
            , DECODE(A.SEGMENT_TYPE,'LOB',0,A.BYTES) NOMAL_COL
            , DECODE(A.SEGMENT_TYPE,'LOB',A.BYTES,0) LOB_COL
        FROM DBA_SEGMENTS A
                , DBA_LOBS B 
        WHERE A.OWNER NOT IN ('OUTLN','MGMT_VIEW','SYS','SYSTEM','EXFSYS','WKSYS','XDB','SI_INFORMTN_SCHEMA','CTXSYS','ANONYMOUS','DMSYS','WK_TEST','WMSYS','ORDSYS','ORDPLUGINS','WKPROXY','MDSYS','SYSMAN','DBSNMP','TSMSYS','DIP','MDDATA','DEMO','AURORA$ORB$UNAUTHENTICATED','AWR_STAGE','CSMIG','DSSYS','LBACSYS','ORACLE_OCM','PERFSTAT','TRACESVR','OLAPSYS')
        AND A.SEGMENT_TYPE IN ('TABLE','LOB')
        AND A.OWNER = B.OWNER(+)
        AND A.SEGMENT_NAME = B.SEGMENT_NAME(+)
    )
    GROUP BY OWNER,TABLE_NAME,TABLESPACE_NAME;
    ```