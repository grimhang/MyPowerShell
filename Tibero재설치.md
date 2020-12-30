# Tibero 를 재 설치 하는 방법
Tibero의 sid명 변경. Tibero 서비스 제거. Tibero 엔진 업그레이드 방법

## 1. Tibero 엔진 업그레이드
Tibero 6.0 기준

* #### 1.1 cmd 관리자 모드로 열기
* #### 1.2 홈디렉터리로 이동
    ```
    C:\> cd %TB_HOME%            # C:\engn001\tibero6 인지 확인
    ```

* #### 1.3 서비스 확인
    ```sql
    C:\> tasklist | find /i "tbsvr"
    ```

* #### 1.4 tip 파일 내용 확인
    ```sql
    C:\> type %TB_HOME%\config\%TB_SID%.tip
    ```

* #### 1.5 라이선스 체크
    ```sql
    C:\> tbboot -l | find /i "expiration"
    ```

* #### 1.6 datafile 위치 확인
    datafile이 엔진 밖에 있는지 확인
    ```sql
    C:\> mkdir c:\patch_work
    C:\> tbsql sys/tibero
    ```

* #### 1.7 DATAFILE 확인. tbsql에서 실행
    ```sql
    SPOOL C:\patch_work\DTDEST.log
    SET LINESIZE 120
    COL FILE_NAME FORMAT A50
    SELECT FILE_NAME FROM DBA_DATA_FILES;
    SPOOL OFF
    
    tbsql exit
    ```

    백업된 데이터파일정보 확인
    ```
    C:\> type C:\patch_work\DTDEST.log
    ```
* #### 1.8 버전 기록 
    ```
    C:\> tbboot -version > c:\patch_work\versionInfo.txt
    C:\> tbboot -cs > c:\patch_work\csInfo.txt

    ```
---
패치 시작
* #### 1.11 tibero down
    ```
    C:\> tbdown immediate
    ```

    tibero 서비스가 내려갔는지 확인
    ```
    C:\> tasklist | find /i "tbsvr"
    ```
* #### 1.12 작업 폴더로 이동
    ```
    C:\> cd %TB_HOME%\..        # C:\engn001 폴더인지 확인
    ```

* #### 1.13 기존 Tibero -> tibero6_bak으로 변경
    ```
    C:\> ren tibero6 tibero6_bak
    ```

* #### 1.14 패치바이너리 압축풀기
    tibero6-bin-FS07_CS_1912-windows64_2008_1-185790-opt-20201026024156.zip 압축풀어서 tibero6 폴더를 C:\engn001 으로 move

* #### 1.15 tibero 설치에 필요한 파일 생성
    ```
    C:\> %TB_HOME%\config\gen_tip.vbs
    C:\> %TB_HOME%\config\gen_psm_cmd.vbs
    ```

* #### 1.16 기존바이이너리에 있던 파일들을 패치바이너리로 덮어쓰기 및 복사
    ```
    C:\> copy tibero6_bak\license\license.xml %TB_HOME%\license\
    C:\> copy tibero6_bak\config\*.tip %TB_HOME%\config\
    C:\> copy tibero6_bak\client\config\*.tbr %TB_HOME%\client\config\
    C:\> copy tibero6_bak\client\config\*.cfg %TB_HOME%\client\config\
    ```

* #### 1.17 ren tibero6_bak tibero6_패치날짜
    ```
    C:\> ren tibero6_bak tibero6_20201211
    ```

* #### 1.18 copy 확인
    ```
    C:\> type %TB_HOME%\config\%TB_SID%.tip
    C:\> type %TB_HOME%\client\config\*.tbr
    ```

* #### 1.19 버전 확인
    ```
    C:\> tbboot -version
    C:\> tbboot -cs
    ```

* #### 1.20 tibero 기동
    ```
    C:\> tbboot
    ```





## 2. Tibero 재설치
이 시나리오에서는 캐릭터셋을 잘못 설정해서 DB를 재설치하는 상황


* #### 2.0 환경변수 확인
    PATH에  C:\engn001\tibero6\bin; C:\engn001\tibero6\client\bin
    존재하는지 확인
    

* #### 2.1 tibero down
    ```
    C:\> tbdown immediate
    ```

* #### 2.2 기존 파일 삭제
    요청서를 참고하셔서 control file, 모든 .dft 파일 , redolog 파일 을 삭제합니다.

    data file이 생성되는 경로("D:/data001/LOG01/data/")에 .passwd 이 파일도 지워 주셔야 합니다.

* #### 2.3 기존 파일 삭제
    제가 %TB_HOME%/config에 db_c.sql을 만들어 놓았습니다.

    db_c.sql을 메모장 or notepad로 character set부분을 수정


* #### 2.4 nomount 모드에서 서비스 기동
    ```
    C:\> tbboot nomount
    ```

* #### 2.5 데이터베이스 생성
    ```
    SQL> @db_c.sql
    ```

* #### 2.6 tibero 기동
    ```
    C:\> tbboot #맞나?
    ```

* #### 2.7 딕셔너리 생성
    cscript //H:CScript 로 바꾼후

    ```
    C:\> cd %TB_HOME%\scripts
    C:\> system.vbs -p1 sys비번 -p2 syscat -a1 y -a2 y -a3 y -a4 y
    C:\> system.vbs -p1 sys비번 -p2 syscat -a1 y -a2 y -a3 y -a4 y
    ```


## 3. Tibero SID 변경

* #### 3.1 tibero down
    ```
    C:\> tbdown immediate
    ```

* #### 3.2 mount 모드로 기동
    ```
    C:\> tbboot mount
    ```    

* #### 3.3 DB명 변경
    ```
    C:\> tbsql sys/tibero
    SQL> alter database rename to "procdb";
    ```

* #### 3.4 DB 중지
    ```
    C:\> tbdown immediate
    ```

* #### 3.5 Tibero 인스턴스 삭제
    ```
    C:\> %TB_HOME%/bin/tbuninstall.vbs

    ----------------------------------------------------------
    Microsoft (R) Windows Script Host 버전 5.812
    Copyright (C) Microsoft Corporation. All rights reserved.

    Warning: TB_SID is not provided.
    TB_SID [PROCDB] will be used.
    TB_HOME = C:\engn001\tibero6
    TB_SID = PROCDB
    service name = Tibero_PROCDB
    Tibero_PROCDB uninstalled successfully.
    ```

* #### 3.6 서비스가 삭제되었는지 확인
    ```ps
    PS C:\> Get-Service tibero*        
    ```

* #### 3.7. %TB_SID%의 환경변수 수정
    ```ps
        PS C:\> $env:tb_sid
            -----------------------
            PROCDB
    
        PS C:\> [Environment]::SetEnvironmentVariable('TB_SID', 'procdb',[EnvironmentVariableTarget]::Machine)    
        PS C:\> [Environment]::GetEnvironmentVariable('TB_SID', 'Machine')
        
        PS C:\$env:tb_sid
            -----------------------
            procdb
    ```

* #### 3.8. tibero install
    ```
    C:\engn001\tibero6\bin > tbinstall.vbs %TB_HOME% %TB_SID%

    ---------------------------------------
    Microsoft (R) Windows Script Host 버전 5.812
    Copyright (C) Microsoft Corporation. All rights reserved.

    TB_HOME = C:\engn001\tibero6
    TB_SID = procdb
    Servce Name = Tibero_procdb
    service account = LocalSystem
    Tibero_procdb installed successfully.
    ```

* #### 3.9. tibero 서비스 상태 확인
    ```ps
    PS C:\gsv tibero*    
        ------------------------
        Tibero_procdb
    ```

* #### 3.10. tip 파일 변경
    %TB_HOME%/config/.tip 파일 변경
    
    a. PROCDB.tip 을 procdb.tip 로 변경. (백업하고)   
    b. tip파일안에 DB_NAME 도 procdb 로 변경

* #### 3.11. tnsname.ora 수정    
    a. tbdsn.tbr 열기
    ```
    C:\> cd %TB_HOME%/client/config/
    C:\> notepad tbdsn.tbr
    ```

    b. DB_NAME=변경후sid 로 변경    
    ```
    #-------------------------------------------------
    # C:\engn001\tibero6\client\config\tbdsn.tbr
    # Network Configuration File.
    # Generated by gen_tip.bat at 11 10 14:43:10      2020
    procdb=(
        (INSTANCE=(HOST=localhost)
                    (PORT=8629)
                    (DB_NAME=procdb)
        )
    )
    ```

* #### 3.12. Tibero 기동
    ```
    C:\> tbboot
    ```

