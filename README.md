# win_service_sample
windows service sample program.

### Monitoring
start DebugView.exe

### Register Service
```
> sc create SAMPLE_SERVICE binPath= C:\hoge\win_service_sample.exe
```

### Start Service
```
> net start SAMPLE_SERVICE
```

### Stop Service
```
> net stop SAMPLE_SERVICE
```

### Delete Service
```
> sc delete SAMPLE_SERVICE
```