#!/usr/bin/python
#coding=utf-8

import xml.etree.ElementTree as ET
import datetime as Time
import csv


def analy_csv(file):
    filename = file
    with open(filename) as f:
        reader = csv.DictReader(f)
        column = [row for row in reader]
    nrow = column[0]
    height = nrow['height']
    location = nrow['location']
    a = [height, location]
    print(height)
    print(location)
    return a


def ai_to_xml(a, directory):
    UII = ET.Element('UIIfindings')

    system = ET.SubElement(UII, 'system')
    namecode = ET.SubElement(system, 'namecode')
    namecode.text = 'uiiview'
    version = ET.SubElement(system, 'version')
    version.text = '1.0'
    time = ET.SubElement(system, 'time')
    datetime = Time.datetime.now().strftime('%Y-%m-%d %H:%M:%S')
    time.text = datetime

    finding = ET.SubElement(UII, 'finding')
    finding.attrib['uid'] = 'patient uid'
    finding.attrib['code'] = 'fracture'
    finding.attrib['name'] = 'L11'

    values = ET.SubElement(finding, 'values')
    summary = ET.SubElement(values, 'summary')
    summary.text = 'input '
    value = ET.SubElement(values, 'value')
    value.attrib['code'] = 'uii'
    value.attrib['name'] = 'postion'
    value.attrib['uint'] = '1.0cm'

    index = ET.SubElement(finding, 'index')
    patient = ET.SubElement(index, 'patient')
    name = ET.SubElement(patient, 'name')
    name.text = 'Patient name'
    id = ET.SubElement(patient, 'id')
    id.text = 'Patient ID'
    study = ET.SubElement(index, 'study')
    uid = ET.SubElement(study, 'uid')
    uid.text = 'Study Instance UID'
    accession_number = ET.SubElement(study, 'accession_number')
    accession_number.text = 'Accession Number'

    keyimage = ET.SubElement(finding, 'kyeimage')
    index = ET.SubElement(keyimage, 'index')
    series = ET.SubElement(index, 'series')
    uid = ET.SubElement(series, 'uid')
    uid.text = 'Series Instance UID'
    number = ET.SubElement(series, 'number')
    number.text = 'Series ID'
    description = ET.SubElement(series, 'description')
    description.text = 'Series Description'
    image = ET.SubElement(index, 'image')
    uid = ET.SubElement(image, 'uid')
    uid.text = 'SOP Instance UID'
    number = ET.SubElement(image, 'number')
    number.text = 'Instance ID'
    file = ET.SubElement(keyimage, 'file')
    codebase64 = ET.SubElement(file, 'codebase64')
    codebase64.text = '......'
    format = ET.SubElement(file, 'format')
    format.text = 'jpg'
    annotations = ET.SubElement(keyimage, 'annnotations')
    annotation = ET.SubElement(annotations, 'annotation')
    data = ET.SubElement(annotation, 'data')

    position = ET.SubElement(data, 'position')
    position.attrib = {'position': a[0]}
    location = ET.SubElement(data, 'location')
    location.text = a[1]
    tname = ET.SubElement(data,'tname')
    tname.text = "biaozhuleixingmingcheng"
    uid = ET.SubElement(data, 'uid')
    uid.text = "biaozhuweiyiUID"

    tree = ET.ElementTree(UII)
    tree.write(directory, encoding='UTF-8')


if __name__ == '__main__':
    x = analy_csv('D:/results.csv')
    ai_to_xml(x, 'D:/a.xml')
