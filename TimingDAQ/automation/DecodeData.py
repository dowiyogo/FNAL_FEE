import os, sys, argparse, subprocess, glob
import ROOT as rt
import numpy as np

def GetCommandLineArgs():
    p = argparse.ArgumentParser()
    p.add_argument('-R', '--runs', type=int, nargs='+', help='List of runs to be processed. If two runs are given: if the order is increasing all the runs in the middle are processed as well, otherwise not.')

    p.add_argument('--data_dir', default='../data')
    p.add_argument('--vVME', default=None, help='Version of the config. Something like vXX (v1, vf1, ...) and has to have a corresponded config file in the config directory (config/config_dir/VME_vXX.config).\n If not given no VME is run.')
    p.add_argument('--vNetScope', default=None, help='Version of the config. Something like vXX (v1, vf1, ...) and has to have a corresponded config file in the config directory (config/config_dir/NetScope_vXX.config).\n If not given no NetScope is run.')

    p.add_argument('--code_dir', default=os.environ['PWD'])
    p.add_argument('--config_dir', default='FNAL_TestBeam_1904/')

    p.add_argument('--no_NimPlus', action='store_true', default=False)
    p.add_argument('--NimPlus_flag', type=str, default='muxout-D')
    p.add_argument('--no_tracks', action='store_true', default=False)
    p.add_argument('--no_Dat2Root', action='store_true')
    p.add_argument('--save_raw', action='store_true',default=False)
    p.add_argument('-f','--force', action='store_true', help='Run even if tracks are not present')
    p.add_argument('--N_max_job', type=int, default=100000)

    #Dat2Root arguments
    p.add_argument('--NO_save_meas', default=False, action='store_true')
    p.add_argument('-N', '--N_evts', type=str, default='0')
    p.add_argument('--draw_debug_pulses', default=False, action='store_true')
    p.add_argument('-v', '--verbose', default=False, action='store_true')
    p.add_argument('--N_skip', type=int, nargs='+')

    return p.parse_args()

if __name__ == '__main__':
    args = GetCommandLineArgs()
    data_dir = args.data_dir
    if not data_dir.endswith('/'):
        data_dir += '/'

    code_dir = args.code_dir
    if not code_dir.endswith('/'):
        code_dir += '/'

    runs_list = []
    if len(args.runs) == 2 and (args.runs[0] < args.runs[1]):
        runs_list = range(args.runs[0], args.runs[1]+1)
    else:
        runs_list = args.runs

    if not args.vVME == None:
        print 'Processing VME'

        output_dir = data_dir + 'VME/RecoData/RecoWithTracks/' + args.vVME
        if not os.path.isdir(output_dir):
            print 'Creating the output directory ', output_dir
            os.mkdir(output_dir)

        for run in runs_list:
            print '========================== Processing Run {0} =========================='.format(run)

            N_expected_evts = -1
            if not args.no_NimPlus:
                NimPlus_file = data_dir + 'NimPlus/TriggerCountNimPlus_{0}.cnt'.format(run)
                if os.path.exists(NimPlus_file):
                    cmd = 'more {0} | grep {1} | awk \'{{print $3}}\''.format(NimPlus_file, args.NimPlus_flag)
                    proc = subprocess.Popen(cmd, stdout=subprocess.PIPE, shell=True)
                    (out, err) = proc.communicate()
                    N_expected_evts = int(out)
                else:
                    print '[WARNING] No NimPlus file present: ' + NimPlus_file

            raw_filename = data_dir + 'VME/RawData/RawDataSaver0CMSVMETiming_Run{0}_0_Raw.dat'.format(run)
            if not os.path.exists(raw_filename):
                if args.verbose:
                    print 'Creating the VME file: ', raw_filename

                matched_files = glob.glob('{0}/RawDataSaver0CMSVMETiming_Run{1}_*_Raw.dat'.format(data_dir + 'VME/RAW', run))

                if len(matched_files) == 0:
                    sys.exit('[ERROR] Unable to find files like: ' +  '{0}/RawDataSaver0CMSVMETiming_Run{1}_*_Raw.dat'.format(data_dir + 'VME/RAW', run))
                elif len(matched_files) > 1:
                    cmd = 'cat ' + ' '.join(matched_files) + ' > ' +raw_filename
                    # print cmd
                    subprocess.call(cmd, shell=True)
                    for f in matched_files:
                        os.remove(f)
                else:
                    cmd = 'mv '+ matched_files[0] + ' ' + raw_filename
                    subprocess.call(cmd, shell=True)
            else:
                print 'VME file found: ', raw_filename

            root_filename = output_dir + '/DataVMETiming_Run{0}.root'.format(run)
            if args.no_Dat2Root:
                print '[INFO] No Dat2Root flag active'
                continue
            if os.path.exists(root_filename) and not args.force:
                print root_filename, 'already present'
                continue

            cmd_Dat2Root = code_dir + 'VMEDat2Root'
            cmd_Dat2Root += ' --input_file=' + raw_filename
            cmd_Dat2Root += ' --config=' + code_dir + 'config/' + args.config_dir + 'VME_{0}.config'.format(args.vVME)
            if args.draw_debug_pulses:
                cmd_Dat2Root += ' --draw_debug_pulses'
            if args.save_raw:
                cmd_Dat2Root += ' --save_raw'
            if args.verbose:
                cmd_Dat2Root += ' --verbose'
            if not args.NO_save_meas:
                cmd_Dat2Root += ' --save_meas'
            if not args.no_tracks:
                tracks_filename = data_dir + 'Tracks/Run{0}_CMSTiming_SlowTriggerStream_converted.root'.format(run)

                if os.path.exists(tracks_filename):
                    cmd_Dat2Root += ' --pixel_input_file=' + tracks_filename
                else:
                    print '[ERROR] Tracks file not found in', tracks_filename
                    if not args.force:
                        print 'If you want to run  without tracks use <--no_tracks> or <--force>.'
                        continue
            if not args.N_skip is None:
                for i,n in enumerate(args.N_skip):
                    cmd_Dat2Root += ' --NSkip{0}={1}'.format(i+1,n)

            N_tot = max(int(args.N_evts), N_expected_evts)
            nj = 1 + N_tot/args.N_max_job
            evt_start_list = np.arange(0,1,1)
            if (N_tot>0):
                evt_start_list = np.arange(0, N_tot, N_tot/float(nj))
                evt_start_list = np.uint32(np.ceil(evt_start_list))

            if evt_start_list.shape[0] == 1:
                cmd_Dat2Root += ' --N_evt_expected=' + str(N_expected_evts)
                cmd_Dat2Root += ' --output_file=' + root_filename
                cmd_Dat2Root += ' --N_evts=' + args.N_evts
                if args.verbose:
                    print '\n'+cmd_Dat2Root
                subprocess.call(cmd_Dat2Root, shell=True)
                # print cmd_Dat2Root
            else:
                print 'Dividing the run into', evt_start_list.shape[0], 'jobs'
                outfile_list = []
                for i in range(evt_start_list.shape[0]):
                    print '\n\n ----------> Job {0}/{1}\n'.format(i+1, evt_start_list.shape[0])
                    aux_name = root_filename.replace('.root', '_{0}.root'.format(i))
                    outfile_list.append(aux_name)

                    aux_cmd = cmd_Dat2Root + ' --output_file=' + aux_name
                    aux_cmd += ' --start_evt=' + str(evt_start_list[i])

                    N_stop = 0
                    if i == evt_start_list.shape[0]-1:
                            N_stop = int(args.N_evts)
                    else:
                        N_stop = evt_start_list[i+1]
                    aux_cmd += ' --N_evts=' + str(N_stop)

                    subprocess.call(aux_cmd, shell=True)

                cmd = 'hadd ' + root_filename + ' ' + ' '.join(outfile_list)
                subprocess.call(cmd, shell=True)
                subprocess.call('rm '+' '.join(outfile_list), shell=True)

                f = rt.TFile.Open(root_filename, 'READ')
                t = f.Get('pulse')
                t.GetEntry(t.GetEntries()-1)
                N_evts_tree =  t.i_evt+1
                f.Close()

                if N_expected_evts != -1 and N_evts_tree != N_expected_evts:
                    print '\n\n\n[ERROR] Number of evts not matching the expected number'
                    print '============= ', N_evts_tree, '!=', N_expected_evts, ' ============'
                    print 'Moving to the currupted directory\n\n\n'
                    corrupted_name = os.path.dirname(root_filename) + '/corrupted/' + os.path.basename(root_filename)
                    subprocess.call('mv ' + root_filename + ' ' + corrupted_name, shell=True)
                elif N_expected_evts == -1:
                    print '[WARNING] Event number matching between trigger and pulse tree not performed'
                else:
                    print '\n\n[INFO] !!!!!! Number of events matching !!!!!\n\n'


            print 'Finished processing run ', run

    if not args.vNetScope==None:
        print 'Processing NetScope'

        output_dir = data_dir + 'NetScope/RECO/' + args.vNetScope
        if not os.path.isdir(output_dir):
            print 'Creating the output directory ', output_dir
            os.mkdir(output_dir)

        for run in runs_list:
            print '========================== Processing Run {0} =========================='.format(run)

            raw_filename = data_dir + 'NetScope/RAW/RawDataNetScope_Run{0}.dat'.format(run)
            if not os.path.exists(raw_filename):
                print '\nNetScope file NOT found: ', raw_filename
                continue
            else:
                print '\nNetScope file found: ', raw_filename

            root_filename = output_dir + '/DataNetScope_Run{0}.root'.format(run)
            if args.no_Dat2Root:
                print '[INFO] No Dat2Root flag active'
                continue
            if os.path.exists(root_filename) and not args.force:
                print root_filename, 'already present'
                continue

            cmd_Dat2Root = code_dir + 'NetScopeDat2Root'
            cmd_Dat2Root += ' --input_file=' + raw_filename
            cmd_Dat2Root += ' --config=' + code_dir + 'config/' + args.config_dir + 'NetScope_{0}.config'.format(args.vNetScope)
            cmd_Dat2Root += ' --output_file=' + root_filename
            cmd_Dat2Root += ' --N_evts=' + args.N_evts
            if args.draw_debug_pulses:
                cmd_Dat2Root += ' --draw_debug_pulses'
            if not args.NO_save_meas:
                cmd_Dat2Root += ' --save_meas'

            print '\n'+cmd_Dat2Root

            subprocess.call(cmd_Dat2Root, shell=True)

            print 'Finished processing run ', run
