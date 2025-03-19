from xml.dom import minidom
import os
from pathlib import Path
import sys

path = os.getcwd()
print(f'Current path: {path}')

period = sys.argv[1]
partition = sys.argv[2]

print('Creating configuration file')

root = minidom.Document()

xml = root.createElement('configuration')
xml.setAttribute('xmlns:xsi', 'http://www.w3.org/2001/XMLSchema-instance')
xml.setAttribute('xsi:noNamespaceSchemaLocation', 'http://sumo.dlr.de/xsd/sumoConfiguration.xsd')
root.appendChild(xml)

inputChild = root.createElement('input')
processingChild = root.createElement('processing')
routingChild = root.createElement('routing')
reportChild = root.createElement('report')
guiOnlyChild = root.createElement('gui_only')

xml.appendChild(inputChild)
xml.appendChild(processingChild)
xml.appendChild(routingChild)
xml.appendChild(reportChild)
xml.appendChild(guiOnlyChild)

# input childs
netfile = root.createElement('net-file')
netfile.setAttribute('value', f'partition_{partition}.net.xml')
routefiles = root.createElement('route-files')

routes = []
for xmlfile in Path(path).glob(f'partition_{partition}_{period}.rou.xml'):
    routes.append(xmlfile.name)
routes_str = ", ".join(str(route) for route in routes)
print(f'Route files: {routes_str}')

routefiles.setAttribute('value',routes_str)


inputChild.appendChild(netfile)
inputChild.appendChild(routefiles)

# processing childs
route_errors = root.createElement('ignore-route-errors')
route_errors.setAttribute('value', 'true')
processingChild.appendChild(route_errors)

# routing childs
steps = root.createElement('device.rerouting.adaptation-steps')
interval = root.createElement('device.rerouting.adaptation-interval')

steps.setAttribute('value', '18')
interval.setAttribute('value', '10')

routingChild.appendChild(steps)
routingChild.appendChild(interval)


xml_str = root.toprettyxml()

with open(f'sim_part_{partition}_{period}.sumocfg', "w") as f:
    f.write(xml_str)

