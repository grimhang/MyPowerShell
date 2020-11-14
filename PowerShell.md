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
    PS C:\> Get-WinEvent -ProviderName "Microsoft-Windows-Kernel-General" | where recordid -e 11134 | fl *
    ```
    
