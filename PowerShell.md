# Powershell 스크립트(Simple)

powershell

## 1. 이벤트 뷰어Tibero ASIS조사 쿼리
ASIS  기준

* #### 1.1 이벤트 뷰어에서 os 시작/종류 시간 확인하기
    ```powershell
    -- 12번은 시작, 13번은 종류
    PS C:\> Get-EventLog System -InstanceId 12,13 -Newest 10 -Source *kernel-general
    ```
