

def thread_signal_read()
    read_txt = IO.readlines("./battery/pid_th")[0] #파일 읽어들이기
    split = read_txt.split(" ")
    pid = split[0]
    threshold = split[1] 
    a = Array.new
    a[0] = pid
    a[1] = threshold
    return a
end



def Notifier 
    p "debug - notifier call"
    pid_th = thread_signal_read() # pid_th 값을 읽어서 pid, threshold 값을 담습니다. 
    pid_num =  pid_th[0].to_i  # Powermanager 의 pid num
    threshold_num = pid_th[1].to_i # Powermanager 의 threshold 값

    battery_value = IO.readlines("./battery/level")[0] # 현재 Battery value (from Emulator )

    if battery_value.to_i > threshold_num
        p "표준모드 호출"
        Process.kill("USR2",  pid_num)
        # Powermanager 에게 USR2 신호 전송
    else
        p "절전모드 호출"
        Process.kill("USR1", pid_num)
        # Powermanager 에게 USR1 신호 전송
    end 
end

 
while true 
    Notifier() # Notifer 함수 1초마다 호출
    sleep 1
end
