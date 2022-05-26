output = "<table><tr><th>Key</th><th>Value</th></tr>"

for k, v in pairs(Info) do
	output = output .. "<tr><td>" .. k .. "</td><td>" .. v .. "</td></tr>"
end
output = output .. "</table>"

HTML_OUT(output)
