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