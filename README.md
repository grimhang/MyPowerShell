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





## 2. Azure SQL 테이블 설계 Best Practice
<kbd>
    <img src="image/02_picture-flow.png" width="500" border="2">  
</kbd>

* #### 2.1 데이터 마이그레이션
    * 데이터 로딩시 권장사항  
        Azure Data Lake Store 또는 Azure Blob Storage에서 Polybase로 DW에 데이터 로드 
    
    * 테이블 디자인 시 기본 권장사항
        * 분산     ==>   라운드 로빈
        * 인덱싱   ==>   힙
        * 분할     ==>   없음
        * 리소스클래스  ==>   largerc 또는 xlargerc

* #### 2.2 테이블 분산시 고려사항
    * a. 테이블 복제     : 디멘션 테이블과 같이 사이즈가 작은 테이블
    * b. 라운드로빈      : 랜덤으로 데이터를 분산시킴
    * c. 해시            : 팩트 테이블 or 대형 차원 테이블과 해시 분산 키  

    * 팁
        * 라운드 로빈으로 시작하지만 대규모 병렬 아키텍처를 활용하는 해시 분산 전략을 목표로 합니다.
        * 공통 해시 키의 데이터 형식이 동일한지 확인합니다.
        * varchar 형식에 배포하지 않습니다. (INT와 datetime 을 권장하나 부득이할 경우 varchar도 가능)
        * 조인 작업이 빈번한 팩트 테이블에 대한 공통 해시 키가 있는 차원 테이블은 해시 분산될 수 있습니다.
        * sys.dm_pdw_nodes_db_partition_stats 를 사용하여 데이터의 모든 왜곡도를 분석합니다.
        * sys.dm_pdw_request_steps 를 사용하여 쿼리의 백그라운드 데이터 이동 분석, 시간 브로드캐스트 모니터링 및 작업 순서 섞기를           수행합니다. 이는 분산 전략을 검토하는 데 유용합니다.  

* #### 2.3 테이블 인덱싱
    * a. 힙
    * b. 클러스터드 인덱스
    * c. 클러스터드 컬럼스토어 인덱스(CCI)

    * 팁
        * 클러스터형 인덱스에 기반하여 필터링에 많이 사용되는 열에 비클러스터형 인덱스를 추가할 수 있습니다.
        * CCI로 테이블에서 메모리를 관리하는 방법에 주의해야 합니다.  
            데이터를 로드할 때 사용자(또는 쿼리)가 큰 리소스 클래스의 이점을 사용하고자 합니다.
        * 다수의 작은 압축 행 그룹을 자르고 만들지 않도록 합니다.
        * Gen2에서 CCI 테이블은 성능을 최대화하기 위해 컴퓨팅 노드에서 로컬로 캐시됩니다.
        * CCI의 경우 행 그룹의 압축 불량으로 인해 성능이 저하될 수 있습니다.
            이 경우 CCI를 다시 작성하거나 다시 구성합니다.
            압축 행 그룹당 10만 개 이상의 행이 필요합니다. 이상적으로는 하나의 행 그룹에 100만 개 행이 있어야 합니다.
        * 증분 로드 빈도 및 크기에 따라 인덱스를 다시 구성하거나 다시 빌드할 때 자동화하고자 합니다.
        * 로우 그룹은 전략적으로 줄입니다. 열려 있는 로우 그룹의 크기는? 앞으로 로드할 데이터의 양은?

* #### 2.4 파티셔닝
    99%의 경우 파티션 키는 날짜를 기반으로 해야 합니다.

* #### 2.5 증분 로드

* #### 2.6 통계 유지 관리

* #### 2.7 성능을 위한 아키텍처 최적화
    허브 & 스포크 아키텍처에서는 MS-SQL 온프레미스 또는 PaaS가 낫다.  


## 3 Azure DW의 제약
클라우드 MPP아키텍처를 가지는 제품들은 공통적으로 다음과 같은 제약을 가진다.  

* #### 3.1 테이블단위의 불가능한 기능
    * a. PRIMARY KEY, FOREIGN KEY, UNIQUE, CHECK constraint
    * b. Unique Index
    * c. 계산된 열 
    * d. 인덱싱된 뷰
        => Material View로 완전 대체 가능
    * e. 시퀀스
        IDENTITY로 대체            
    * f. 시노님  
        뷰를 이용해 대체	

    * g. 트리거
    * h. 사용자 정의 데이터 타입
    * i. 테이블 형식 변수 ⇒ 임시 테이블
    * j. 트랜잭션은 무조건 read uncommitted  
        ACID 트랜잭션을 지원하긴 하나 dirty read가 되기 때문에 주의 필요.

    * k. 커서 지원 안함  
        WHILE문으로 변경 필요

* #### 3.2 데이터베이스 제약
    * a. 데이터베이스간 조인 불가  
        기존에는 업무 기준으로 데이터베이스를 분리하였다면  
        이제는 한 데이터베이스에 몰아넣고 스키마 기반으로 업무를 구분
    * b. 스키마와 데이터베이스 롤 조사 필요


## 4. Azure DW 테이블 설계 전략
데이터 분산 전략 Distribution 파악하기  
* 파티션 :  1개 서버에서 데이터를 디스크에 분산시  
* 분산    : 전체 60개의 서버중 어느 서버에 1개의 로우를 저장할지  

분산을 먼저 결정하고 이후 파티션을 정하는 방향으로 테이블 설계  

* #### 4.1 분산 종류
    * a. Hash  
        특별한 해시함수를 통해서 특정 노드로 데이터 분산
        > 예) 해시함수를 상품번호(ProductNO) 기준으로 정하면  
            주문테이블의 1235133 이라는 상품번호가 들어있는 로우와  
            상품테이블의 1235133 이라는 로우가 같은 노드에 위치하게됨.  
            조인시 다른 노드에서 데이터를 가져올 필요 없기에 성능 저하 없음.

    * b. round robin  
        그냥 랜덤

    * c. 복제  
        코드테이블과 같이 크기도 작고 모든 노드에서 필요할 경우 60개 노드 모두에 데이터 복제

    만약 상품번호로 조인하는 두개의 테이블 데이터가 다른 노드에 있으면
    DMS(데이터 이동 서비스)가 작동되기에 네트워크를 통해 다른 노드에 데이터를 내 메모리로 먼저 복사해오는 작업이 선행되서 성능 저하. 적절한 분산 전략은 매우 중요.

* #### 4.2 컬럼스토어 인덱스 전략
    컬럼베이스 저장 구조이기 때문에 SQL에서 필요한 컬럼만 나열하는게 매우 중요.  
    남,여처럼 선택도가 매우 나쁜 데이터들은 단 2건만 저장하면 되기에 성능과 저장공간 획기적으로 절감

* #### 4.3 테이블 전략 권장사항
    * a. 팩트 : Hash전략을 쓰고 클러스터더 컬럼스토어 인덱스
    * b. 차원 : 복제전략을 쓰고 , 일반적으로 작은 테이블. 인덱스는 아무거나
    * c. 스테이징 : 라운드로빈 전략을 쓰고 팩트나 차원으로 데이터 이용시 CTAS나
                INSERT….. SELECT 사용. 인덱스는 Heap사용

* #### 4.4 스키마 베이스 업무 구분
    멀티데이터베이스가 아니기에 1개의 데이터베이스를 만들고 오라클처럼 스키마를  
    업무 구분 경계로 만들어서 스키마별 테이블을 생성.  

* #### 4.5 테이블 파티셔닝
    https://github.com/grimhang/AzurePartition  참고  

    이미 데이터가 노드단위로 분산되어 있기 때문에  파티셔닝의 중요도가 떨어짐.  
    너무 많은 파티션 만들지 않게 조심필요.  
    Range 파티션만 구현 가능하고 주로 날짜와 관련된 컬럼을 기준으로 Range파티션 세팅

        MSSQL과 Azure DW의 파티션 생성 방법의 차이
        ------------------------------------------
        * a. MS-SQL Server
            * 파티션 함수 생성
            * 파티션 스키마 생성
            * 테이블 생성
            * 테이블과 파티션 스키마 연결

        * b. Azure DW
            * 테이블 생성시 파티션 분리 기준을 같이 지정해 한번에 생성

        만약 파티션 구조가 변경이 되어야 한다면 CTAS를 이용해 새로 만들고 기존 테이블 DROP해야함

* #### 4.6 통계
    자동 생성 지원. 업데이트 기능 GA됨
    

## 5. Azure DW 데이터 로딩 전략
* #### 5.1 외부 데이터 로딩
    기존에 MS-SQL 에는 SSIS(ETL툴), BCP(일괄배치입력) 등을 통해 입력.
    Azure DW에는 polybase 기능을 쓸수 있게 Azure Storage 서비스(Azure Blob Storage, Azure Data Lake)에 데이터를 일단 적재하고 그 후에 병렬로 DW에 로딩할수 있게 한다. 

    Azure Storage, Azure Data Lake는 csv와 같은 파일과 하둡같은 데이터를 저장하는 서비스  
    그 이후 Polybase기능과 CTAS를 이용해 적재 권장

* #### 5.2 컬럼스토어 설정

* #### 5.3 Azure Data Factory
    Azure에서 제공하는 ETL 기능


## 6. Azure DW 쿼리
기본적으로 MS-SQL 서버와 쿼리 호환성은 뛰어나서 대부분의 쿼리 기능이 동일

* #### 6.1 쿼리에 Lable 옵션 적용
    다음과 같이 쿼리를 작성해서 튜닝등의 목적으로 차후 추적 용이
    ```sql
    SELECT *
    FROM [ext].[dimension_City]
    OPTION (LABEL = 'CTAS : Load [wwi].[dimension_City]')
    ```

* #### 6.2 시퀀스가 지원이 안되고 IDENTITY 사용
    컬럼의 속성으로 지정하면 자동으로 증가된 값이 적용됨
    ```sql
    CREATE TABLE dbo.T1
    (   
        C1      INT IDENTITY(1,1) NOT NULL
        , C2    INT NULL
    );
    
    INSERT INTO T1 (C2) VALUES (111);
    INSERT INTO T1 (C2) VALUES (222);

    -- 결과는 다음과 같이 나옴\\
    T1            T2
    ------------  ---
    1             111
    2             222
    ```
* #### 6.3 확장 그룹핑 기능 사용 가능
    * a. GROUP BY with ROLLUP
    * b. GROUPING SETS
    * c. GROUP BY with CUBE


## 7. Azure DW Deploy 전략
    개발, 테스트, 실서버  총 3개의 인스턴스 서비스를 구상 - 하고 있는데 테스트는  
    항상 필요한게 아니기 때문에 실제 필요한 때만 그때 그때 인스턴스를 생성하고
    끝나면 종료하는 식으로 비용 절감.  


## 8. Azure DW 모범 사례
* a. Insert문을 일괄처리로 그룹화
* b. Polybase를 사용하여 데이터를 신속히 로드
* c. 팩트테이블에 해시 분산 테이블 사용
* d. MPP는 이미 분산단계에서 상당부분 데이터가 흩어져 있기 때문에 과도한 파티션 자제
* f. 가능한한 작은 열크기 사용
* g. 임시 테이블 사용 NOT 테이블변수


## 9. Azure DW 성공의 필수 선행 조건

* #### 9.1 표준화 준수율 100% 와 ASIS 데이터 정제 선행 필수
    MPP 머신상에 존재하는 테이블들끼리 조인을 할 경우 같은 데이터 타입이어야 함.   
    그러기에 강력한 표준화 준수가 필요하고 결정된 표준화 규칙대로 ASIS 데이터들을 정제까지 해야 합니다.    
    그래야 MPP 성능이 잘 나옴  

    예) 만약 상품번호가 한쪽은 int고 다른 한쪽은 bigint 일 경우 조인을 하면 한쪽이 형변환이 먼저 발생해서  
    Azure DW의 분산(distribution)이 흐트러지고 재분산되기 때문에 성능에 심각한 문제 발생

* #### 9.2 스타 스키마 모델링 준수
    MPP 구조에서는 테이블 분산 전략이 중요하기 때문에 여러 테이블들을 조인하여 결합된 형태의 팩트테이블로 존재해야 함.  
    그래야만 분산키를 1~3 개 내외에서 선택할수 있음.
    그렇지 않고 기존의 정규화된 ERD 형태의 기존 모델(허브 & 스포크 모델)이면 여러 테이블들을 조인을 해야 하고  
    각각의 테이블 별로 분산키를 선택해야 하는데 심하게는 10개가 넘는 분산키중에 선택을 해야 할수 있음. 
    이런 경우 적절한 분산키 선택은 불가능

* #### 9.3 Azure DW MPP 구조에 지식을 가지고 있는 모델러
    MPP구조의 이득을 얻기 위해서는 팩트 테이블 분산 전략이 가장 중요한데 업무 모델러들은 해당 개념을 대부분 모름.  
    하지만 테이블 분산키를 선정해야 하는 것은 업무 모델러 역할이기 때문에 이해 필수.

* #### 9.4 Azure DW에 능숙한 DBA
    데이터베이스 생성부터 테이블 생성, 데이터 로딩, 이중화, 최종 단계의 튜닝까지  
    모든게 기존 RDB와 개념부터 많이 다르다. 능숙한 DBA가 있어야 원활하게 수행할수 있다.


## 10. 총평
맨 처음에 PK, FK, Unique도 안되는 미완성 제품으로 생각되었으나 하지만 조사를 하면 할수록 대용량 데이터를 효과적으로 분석할수 있는 제품이라고 생각이 변하게 되었습니다. 하지만 도입하기에 고려해야 할 조건이 상당히 많아 어려움도 상당합니다.

MPP 구조를 가진 다른 클라우드 경쟁 제품(AWS Redshift, Snowflake.net, Google Big Query, Alibaba Cloud Max)들도 동일한 제약을
가지고 있고 아키텍처 구조의 특징이기 때문에 이런 점을 이해해야 성공적인 프로젝트가 될수 있습니다.