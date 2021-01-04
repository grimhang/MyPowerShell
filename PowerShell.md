# Powershell 스크립트(Simple)

powershell

## 1. 이벤트 뷰어
ASIS  기준

* #### 1.1 이벤트 뷰어에서 os 시작/종료 시간 확인하기
    ```powershell
    -- 12번은 시작, 13번은 종료
    PS C:\> Get-EventLog System -InstanceId 12,13 -Newest 10 -Source *kernel-general
    ```

    ```powershell
    -- 위에가 잘 안되면 
    PS C:\> Get-WinEvent -ProviderName "Microsoft-Windows-Kernel-General" `
      | where id -in 12,13 | select timecreated, RecordID, Message -First 10 | ft -AutoSize
    ```

    ```powershell
    -- 해당 번호로 상세 확인
    PS C:\> Get-WinEvent -ProviderName "Microsoft-Windows-Kernel-General" | where recordid -eq 11134 | fl *
    ```
    
## 2. 프로세스 검사
* #### 2.1 가장 메모리 사용이 큰 Top 10 프로그램들의 리스트 텍스트로 뽑기
  ```powershell
    PS C:\> ps | sort WS -desc | select Name, @{n="WS(MB)";e={"{0,13:N0}" -f ($_.WS/1MB)}} -First 10 | Out-File pslist.txt
  ```

  ```powershell
    PS C:\> ps | sort WS -desc | ft name, id, @{n='VM(MB)';e={$_.vm/1MB};formatstring='N2';align='right'} -AutoSize
  ```

## 3. 환경변수
* #### 3.1 환경변수 설정 창 열기
  sysdm.cpl, 3

  시스템 환경변수 조회
  ```
  C:\> reg query "HKEY_LOCAL_MACHINE\SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
  PS C:\> ($env:Path).Split(";")
  ```

  사용자 환경변수 조회
  ```
  C:\> reg query HKEY_CURRENT_USER\Environment
  ```

* #### 3.2 환경변수 신기한점
  a. 환경변수가 적용되려면 해당 계정을 로그아웃/로그인 대부분필요.
  b. PS C:\> ($env:Path).Split(";") 가 젤 확실

## 4. 기타
  ```ps
    gsv | Get-Member
    help 명령어 -Full
    PS C:\> gsv Tibero* | fl s*
    PS C:\> Set-Service -Name Tibero_FERT -StartupType Automatic
    
    PS C:\> gps | measure ws -sum | select {Sum/1KB}
    PS C:\> gps | where {$_.ProcessName -ne 'svchost'}
    PS C:\> gps | sort WS -Descending | more
    PS C:\> gps | sort WS -Descending | select -first 10
    PS C:\> gps | where {($_.ProcessName -ne 'svchost') -and ($_.ProcessName -ne 'conhost')}
    
    PS C:\> Get-Process | Sort-Object WS -Descending | Select-Object Name, @{n='WorkingSet Memory (MB)'; e={ '{0:N2}' -f ($PSItem.WS / 1MB)}} -First 10
    PS C:\> ps | Sort WS -Desc | Select Name, @{n='WorkingSet Memory (MB)'; e={ '{0:N2}' -f ($_.WS / 1MB)}} -First 10 #-f옵션은 select에서만 가능

    PS C:\> ps | sort WS -desc | ft name, id, @{name='VM(MB)';expression={$_.VM/1MB};formatstring='N2';align='right'} -AutoSize
  ```

