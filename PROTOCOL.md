frontend -> cliente
{ "cmd":"init", "name":"Luca", "ip":"127.0.0.1", "port":"9191" }
{ "cmd":"send", "text":"hola" }
{ "cmd":"quit" }


cliente -> frontend
{ "type":"conexion", "server":"127.0.0.1", "port":9191 }
{ "type":"mensaje", "de":"Luca", "text":"hola" }
{ "type":"error", "msg":"..." }



