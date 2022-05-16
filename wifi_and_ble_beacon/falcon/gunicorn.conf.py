bind = "0.0.0.0:8000"

max_requests = 1000
timeout = 30
keep_alive = 2

accesslog = '/data/accesslog.log'
loglevel = 'warning'
access_log_format = '%(h)s %(l)s %(u)s %(t)s "%(r)s" %(s)s %(b)s "%(f)s" "%(a)s"'