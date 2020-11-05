# Tibero 조사 쿼리(Simple)

Chem의 티베로의 조사를 하는 쿼리   

## 1. Tibero ASIS조사 쿼리
ASIS Tibero 5.0 기준

* #### 1.1 인스턴스 조사
    ```sql
    -- 인스턴스 정보
    SELECT * FROM V$INSTANCE;

    -- DB정보
    SELECT * FROM V$DATABASE;

    -- 빌드버전 확인
    SELECT * FROM V$VERSION;

    -- 데모인지 아닌지
    SELECT * FROM V$LICENSE;
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
    ORDER BY DECODE(USERNAME,'SYS',2,'SYSCAT',2,'SYSGIS',2,'OUTLN',2,'WMSYS',2,'TIBERO',2,'TIBERO1',2,1)
        , USERNAME;
    ```

* #### 1.8 계정별 권한
    ```sql
    -- a. 시스템 권한   
    SELECT * FROM DBA_SYS_PRIVS WHERE GRANTEE = 'SYS' ;

    -- b. 사용자에게 부여된 롤 확인(시스템 권한이 롤에 포함됨)
    SELECT * FROM DBA_ROLE_PRIVS WHERE GRANTEE = 'SYS' ;

    -- c. 사용자에게 부여된 롤에 부여된 시스템 권한 확인
    SELECT * FROM DBA_SYS_PRIVS WHERE GRANTEE = 'SYS' ;

    -- d. 타 사용자에게 부여한 객체(테이블등) 권한 확인
    SELECT * FROM DBA_TAB_PRIVS WHERE OWNER = 'SYS' ;
    ```


* #### 1.9 계정별 용량
    ```sql
    SELECT owner , sum(bytes/1024/1024) SizeMB
    FROM dba_segments
    GROUP BY owner ;
    ```

* #### 1.10 테이블별 용량
    ```sql
    SELECT T.OWNER, T.TABLE_NAME, T.TABLESPACE_NAME, T.NOMAL_COL_SIZE_MB, T.LOB_COL_SIZE_MB
        , DECODE(C.CONSTRAINT_TYPE, 'P', 'Y', 'N') PK_YN
    FROM 
    (
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
        GROUP BY OWNER,TABLE_NAME,TABLESPACE_NAME
    ) T
        LEFT JOIN DBA_CONSTRAINTS C  	ON T.OWNER = C.OWNER AND T.TABLE_NAME = C.TABLE_NAME AND C.CONSTRAINT_TYPE = 'P';
    ```

* #### 1.11 오브젝트 종류별 갯수
    ```sql
    SELECT OWNER, OBJECT_TYPE, COUNT(*) CNT
    FROM DBA_OBJECTS 
    WHERE OWNER NOT IN ('SYS', 'OUTLN', 'WMSYS', 'SYSGIS', 'SYSCAT', 'PUBLIC')
    GROUP BY OWNER, OBJECT_TYPE
    ORDER BY OWNER, OBJECT_TYPE
    ```

* #### 1.12 테이블 로우수 구하기
    통계 업데이트 선행되어야 제대로 보인다.

    ```sql
    SELECT A.OWNER AS "소유자", A.OBJECT_NAME AS "오브젝트명" 
        , B.NUM_ROWS AS "ROWS"
    FROM dba_objects AS A
        JOIN all_tables AS B
            ON  A.OWNER = B.OWNER AND A.object_name = B.table_name
    WHERE A.OWNER IN ('계정')
    ORDER BY A.OWNER, A.OBJECT_NAME; 
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
        DDL스크립트를 표시 여부 (기본값: N)

    * c. IP   
        Export 대상 Tibero 서버의 IP 주소를 입력한다. (기본값: localhost)

    * d. USER   
        사용자 모드로 Export를 수행할 때 Export될 객체의 소유자를 지정

    * e. GRANT   
        Export를 수행할 때 권한의 Export 여부를 지정(default: Y)        

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


## 3. Tibero 내장 기본 계정

* #### 3.1 계정       
   
    |계정|설명|
    |------|---|
    |SYS|오라클 내장 root 권한 기본 계정|
    |SYSCAT|딕셔너리를 관리하는 내장 계정|
    |OUTLN|동일한 SQL을 수행할 때 항상 같은 질의 플랜(plan)으로 수행될 수 있게 관련 힌트(hint)를 저장|
    |SYSGIS|GIS(Geographic Information System)와 관련된 테이블 생성 및 관리를 하는 계정|
    |TIBERO|CONNECT, RESOURCE, DBA 역할이 부여된 본보기 사용자 계정|
    |TIBERO1|CONNECT, RESOURCE, DBA 역할이 부여된 본보기 사용자 계정|

## 4. Tibero 네트워크 제어

* #### 4.1 전체 네트워크 제어
    데이터 로딩시 권장사항  
    
    ```
    SQL> ALTER SYSTEM LISTENER REMOTE OFF;
    ```
    * 해당 서버 로컬에서만 접속 가능. tbdsn.tbr에 IP가 localhost가 설정된 경우만
    * 이미 접속된 세션은 계속 유지.

* #### 4.2 IP 기반 네트워크 제어
    파라메터파일(tip)에 ip리스트를 추가
    
    ```
    LSNR_INVITED_IP=10.10.1.1;10.10.1.2
    ```    
    * localhost, 10.10.1.1, 10.10.1.2   오직 이 3개의 IP에서만 접속 가능
    


