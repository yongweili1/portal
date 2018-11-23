import os
import time
import subprocess


class CMoveScu(object):
    def __init__(self, pacs_ae_title, pacs_ip, pacs_port,
                 client_ae_title, client_port, save_dir):
        self.pacs_ae_title = pacs_ae_title
        self.pacs_ip = pacs_ip
        self.pacs_port = pacs_port
        self.client_ae = client_ae_title
        self.client_port = client_port
        self.save_dir = save_dir

    def movescu_by_studyuid(self, uid):

        ab_path = os.path.abspath(self.save_dir)
        ab_path = os.path.join(ab_path, uid)
        if not os.path.exists(ab_path):
            os.makedirs(ab_path)

        cmd = 'movescu -v -aec {0} -aet {1} -aem {2} --port {3} -od {4} {5} {6} ' \
              '-k QueryRetrieveLevel=STUDY  -k StudyInstanceUID={7}' \
            .format(self.pacs_ae_title, self.client_ae, self.client_ae,
                    self.client_port, ab_path, self.pacs_ip,
                    self.pacs_port, uid)
        print cmd
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
        out, err = p.communicate()
        print out
        print err
        return err

    def movescu_by_seriesuid(self, study_uid, series_uid):
        ab_path = os.path.abspath(self.save_dir)
        ab_path = os.path.join(ab_path, series_uid)
        if not os.path.exists(ab_path):
            os.makedirs(ab_path)

        cmd = 'movescu -v -aec {0} -aet {1} -aem {2} --port {3} -od {4} {5} {6} ' \
              '-k QueryRetrieveLevel=STUDY -k StudyInstanceUID={7} ' \
              '-k QueryRetrieveLevel=SERIES -k SeriesInstanceUID={8}' \
            .format(self.pacs_ae_title, self.client_ae, self.client_ae,
                    self.client_port, ab_path, self.pacs_ip,
                    self.pacs_port, study_uid, series_uid
                    )
        print cmd
        p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True, close_fds=True)
        p.communicate()
        return p.returncode

    def movescu_by_imageuid(self, study_uid, series_uid, image_uids, retrieve_interval):
        ab_path = os.path.abspath(self.save_dir)
        ab_path = os.path.join(ab_path, series_uid)
        if not os.path.exists(ab_path):
            os.makedirs(ab_path)

        for image_uid in image_uids:
            cmd = 'movescu -v -aec {0} -aet {1} -aem {2} --port {3} -od {4} {5} {6}  ' \
                  '-k QueryRetrieveLevel=STUDY -k StudyInstanceUID={7} ' \
                  '-k QueryRetrieveLevel=SERIES -k SeriesInstanceUID={8} ' \
                  '-k QueryRetrieveLevel=IMAGE -k SOPInstanceUID={9} ' \
                .format(self.pacs_ae_title, self.client_ae, self.client_ae,
                        self.client_port, ab_path, self.pacs_ip,
                        self.pacs_port, study_uid, series_uid, image_uid)
            # print cmd
            p = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
            p.communicate()
            if p.returncode == 0:
                print image_uid
                # if retrieve_interval > 0:
                #     time.sleep(retrieve_interval)
            else:
                print 'error for move by image uid: ', image_uid
                break
        return p


if __name__ == '__main__':
    a = CMoveScu('med_imFIR', '192.168.231.18', '104', 'ceshi', '3333', r'/home/uii/PKU_Pro_Final/cmove_data')

    # b = a.movescu_by_studyuid('1.2.840.113564.19216823112.534.20181023115440500.121')

    c = a.movescu_by_seriesuid('1.2.840.113564.19216823112.534.20181023115440500.121',
                               '1.2.840.113564.19216823350.2018102315240588290')

    d = a.movescu_by_imageuid('1.2.840.113564.19216823112.534.20181023115440500.121',
                              '1.2.840.113564.19216823350.2018102315240588290',
                              '1.2.840.113564.19216823350.2018102315240588210.1203801020003')
