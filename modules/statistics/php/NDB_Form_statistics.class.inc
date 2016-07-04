<?php
/**
 * The welcome menu
 * @package main
 */
class NDB_Form_statistics extends NDB_Form
{
    function _hasAccess() {
        $user =& User::singleton();
        return $user->hasPermission('data_entry');
    }

    function _inCenter($centerID, $Centres) {
        foreach ($Centres as $Centre) {
            if($Centre['NumericID'] == $centerID) {
                return TRUE;
            }
        }
        return FALSE;
    }

    function render_stats_table($header, $subcats, $visits, $dropdown_name, $dropdown_opt, $dropdown_selected, $centres, $data, $Subsection="",$projectID=null) {
        $tpl_data = array();
        $tpl_data['test_name'] = $_REQUEST['test_name'];
        $tpl_data['Subsection'] = $Subsection;
        $tpl_data['Visits'] = $visits;
        $smarty = new Smarty_neurodb("statistics");
        $tpl_data['Header'] = $header;
        $tpl_data['Subcategories'] = $subcats;
        $tpl_data['Subprojects'] = Utility::getSubprojectsForProject($projectID);
        $tpl_data['DropdownName'] = $dropdown_name;
        $tpl_data['DropdownOptions'] = $dropdown_opt;
        $tpl_data['DropdownSelected'] = $dropdown_selected;
        $tpl_data['Centers'] = $centres;
        foreach($data as $row) {
            $subproj = $row['SubprojectID'];
            $vl = $row['VLabel'];
            $subcat = $row['Subcat'];
            $center = $row['CenterID'];


            if(array_search($vl, $visits) !== FALSE && array_search($subcat, $subcats) !== FALSE && $this->_inCenter($center, $centres) !== FALSE) {
                $tpl_data['data'][$subproj][$vl][$subcat] += $row['val'];
                $tpl_data['data'][$subproj][$vl]['total'] += $row['val'];
                $tpl_data['data'][$subproj][$subcat] += $row['val'];
                $tpl_data['data'][$vl][$subcat] += $row['val'];
                $tpl_data['data'][$vl]['total'] += $row['val'];
                $tpl_data['data'][$subproj]['C' . $center][$vl][$subcat] = $row['val'];
                $tpl_data['data']['Cat' . $subcat] += $row['val'];
                $tpl_data['data'][$subproj]['C' . $center][$subcat] += $row['val'];
                $tpl_data['data']['C' . $center][$vl][$subcat] += $row['val'];
                $tpl_data['data'][$subproj]['total'] += $row['val'];
                $tpl_data['data']['C' . $center][$subcat] += $row['val'];
                $tpl_data['data']['total'] += $row['val'];
            }

        }
        $smarty->assign($tpl_data);
        $html = $smarty->fetch("table_statistics.tpl");
        return $html;

    }

    function statistics()
    {
        $DB =& Database::singleton();

        $this->tpl_data['StatsTabs'] = $DB->pselect(
            "SELECT ModuleName, SubModuleName, Description
                 FROM StatisticsTabs
                 ORDER BY OrderNo",
            array()
        );
    }

    function stats_general()
    {
        //$DB =& Database::singleton();
    }

    function stats_behavioural()
    {
        $DB =& Database::singleton();
        $DB->select("SELECT CONCAT('C', CenterID) as ID, CenterID as NumericID, PSCArea as LongName, Name as ShortName FROM psc WHERE CenterID <> '1' AND Study_site = 'Y'", $centers);

        $this->tpl_data['Centers'] = $centers;
        $this->tpl_data['Sites'] = array('' => 'All Sites');

        $projects[null] = 'All Projects';
        foreach(Utility::getProjectList() as $key=>$value) {
            $projects[$key] = $value;
        }
        $this->tpl_data['Projects'] = $projects;
        foreach ($centers as $row) {
            $this->tpl_data['Sites'][$row['NumericID']] = $row['LongName'];
            if($_REQUEST['site'] == $row['NumericID']) {
                $this->tpl_data['CurrentSite'] = array('ID' => $row['NumericID'], 'Name' => $row['LongName']);
            }

        }

        if(isset($_REQUEST['BehaviouralProject']) && $_REQUEST['BehaviouralProject'] != '') {
            $ExtraProject_Criteria = ' AND (c.ProjectID IS NULL OR c.ProjectID=' . $_REQUEST['BehaviouralProject'] . ')';
            $currentProject = $_REQUEST['BehaviouralProject'];
            $this->tpl_data['CurrentProject'] = array('ID' => $currentProject, 'Name' => $projects[$currentProject]);
        } else {
            $ExtraProject_Criteria = '';
        }
        $subprojects = Utility::getSubprojectsForProject($currentProject);

        $this->tpl_data['Subprojects'] = $subprojects;
        foreach($subprojects as $key=>$val) {
            $subprojList .= $key.",";
        }
        $subprojList = substr($subprojList, 0, -1);
        $config = NDB_Config::singleton();

        $Visits = Utility::getExistingVisitLabels($currentProject);
        $this->tpl_data['Visits'] = $Visits;

        //---- BEHAVIORAL STATS -----
        $mri_data = array();
        $DB->select("SELECT s.CenterID,
                f.Data_Entry as Data_Entry,
                s.visit_label as VLabel,
                COUNT(s.CandID) as val
                FROM session s JOIN candidate c ON (s.CandID=c.CandID)
                JOIN flag f ON (f.SessionID=s.ID)
                WHERE 
                s.Active='Y' AND c.Active='Y' 
                AND s.Current_stage <> 'Recycling Bin'
                AND f.CommentID NOT LIKE 'DDE%' 
                AND s.SubprojectID IN ($subprojList)
                $ExtraProject_Criteria
                GROUP by s.CenterID, VLabel, f.Data_Entry",
            $result);

        foreach($result as $row) {
            // Put some things into variables to make the lines shorter
            // so that they don't wrap. Note that "c"(enter array) needs to 
            // be a reference since we'll be modifying it.
            $center = $row['CenterID'];
            $vl = $row['VLabel'];
            $c =& $this->tpl_data['behaviour']['C' . $center];

            $c[$vl]['total'] += $row['val'];
            $c['all']['total'] += $row['val'];
            if($row['Data_Entry'] == 'Complete') {
                $c[$vl]['complete'] += $row['val'];
                $c['all']['complete'] += $row['val'];
            }
            if($c[$vl]['total'] != 0) {
                $c[$vl]['percent'] =
                    floor($c[$vl]['complete'] / $c[$vl]['total']*100);
            }
            if($c['all']['total'] != 0) {
                $c['all']['percent'] =
                    floor($c['all']['complete'] / $c['all']['total']*100);
            }
        }
        // DDE STATS
        $ddeInstruments = $config->getSetting('DoubleDataEntryInstruments');
        $ddeInstrumentsQuoted = array();
        foreach ($ddeInstruments as $ddeInstrument) {
            array_push($ddeInstrumentsQuoted, $DB->quote($ddeInstrument));
        }

        $ddeInstrumentCriteria = "";
        if ($ddeInstruments != null) {
            $ddeInstrumentCriteria = implode(" OR f.Test_name=", $ddeInstrumentsQuoted);
            $ddeInstrumentCriteria = "AND (f.Test_name =" . $ddeInstrumentCriteria . ") ";
        }
        
        $DB->select("SELECT s.CenterID, 
                f.Data_Entry as Data_Entry, 
                s.visit_label as VLabel,
                COUNT(s.CandID) as val
                FROM session as s JOIN candidate as c ON (s.CandID=c.CandID) JOIN flag as f ON (f.SessionID=s.ID) 
                WHERE s.Active='Y' 
                AND s.Current_stage <> 'Recycling Bin'
                AND f.CommentID LIKE 'DDE%' 
                AND c.Active='Y' 
                $ddeInstrumentCriteria
                $ExtraProject_Criteria
                GROUP BY s.CenterID, VLabel, f.Data_Entry ORDER BY c.PSCID;",
            $result);
        foreach($result as $row) {
            $center = $row['CenterID'];
            $vl = $row['VLabel'];
            $c =& $this->tpl_data['dde']['C' . $center];

            $c[$vl]['total'] += $row['val'];
            $c['all']['total'] += $row['val'];
            if($row['Data_Entry'] == 'Complete') {
                $c[$vl]['complete'] += $row['val'];
                $c['all']['complete'] += $row['val'];
            }
            if($c[$vl]['total'] != 0) {
                $c[$vl]['percent'] =
                    floor($c[$vl]['complete'] / $c[$vl]['total']*100);
            }
            if($c['all']['total'] != 0) {
                $c['all']['percent'] =
                    floor($c['all']['complete'] / $c['all']['total']*100);
            }
        }
    }

    function stats_MRI()
    {
        $DB =& Database::singleton();

        $projects[null] = 'All Projects';
        foreach(Utility::getProjectList() as $key => $value) {
            $projects[$key] = $value;
        }
        if(isset($_REQUEST['site']) && $_REQUEST['site'] != '') {
            $ExtraSite_Criteria = 'AND (c.CenterID IS NULL OR c.CenterID=' . $_REQUEST['site'] . ')';
        } else {
            $ExtraSite_Criteria = '';
        }
        $currentProject = null;
        $this->tpl_data['showTable']= true;
        if(isset($_REQUEST['MRIProject']) && $_REQUEST['MRIProject'] != '') {
            $ExtraProject_Criteria = ' AND (c.ProjectID IS NULL OR c.ProjectID=' . $_REQUEST['MRIProject'] . ')';
            $currentProject = $_REQUEST['MRIProject'];
            $this->tpl_data['CurrentProject'] = array('ID' => $currentProject, 'Name' => $projects[$currentProject]);
        } else {
            $ExtraProject_Criteria = '';
        }
        $subprojects = Utility::getSubprojectsForProject($currentProject);
        $this->tpl_data['Subprojects'] = $subprojects;
        foreach($subprojects as $key=>$val) {
            $subprojList .= $key.",";
        }
        $subprojList = substr($subprojList, 0, -1);
        $this->tpl_data['Projects'] = $projects;

        $this->tpl_data['Subprojects'] = $subprojects;
        $this->tpl_data['Projects'] = $projects;

        $DB->select("SELECT CONCAT('C', CenterID) as ID, CenterID as NumericID, PSCArea as LongName, Name as ShortName FROM psc WHERE CenterID <> '1' AND Study_site = 'Y'", $centers);
        $this->tpl_data['Centers'] = $centers;
        $this->tpl_data['Sites'] = array('' => 'All Sites');
        foreach ($centers as $row) {
            $this->tpl_data['Sites'][$row['NumericID']] = $row['LongName'];
            if($_REQUEST['site'] == $row['NumericID']) {
                $this->tpl_data['CurrentSite'] = array('ID' => $row['NumericID'], 'Name' => $row['LongName']);
            }
        }
        $this->tpl_data['Subprojects'] = Utility::getSubprojectList(); //array('1' => '6 Month Recruit', '2' => '12 Month Recruit', '3' => 'Control');
        $Visits = Utility::getExistingVisitLabels($currentProject);
        $this->tpl_data['Visits'] = $Visits;


        $MRISubcategories = array('Complete', 'Partial Run', 'No Scan');
        $MRIOptions = array('T1' => 'T1',
                            'FLAIR' => 'FLAIR',
                            'DTI64' => 'DTI 64',
                            'Fieldmap_DTI' => 'Fieldmap DTI',
                            'Fieldmap_BOLD' => 'Fieldmap BOLD',
                            'BOLD_Encoding_Enrollment' => 'BOLD Encoding_Enrolment',
                            'BOLD_Retrieval_Enrollment' => 'BOLD Retrieval_Enrolment',
                            'BOLD_Resting' => 'BOLD Resting',
                            'BOLD_Encoding' => 'BOLD Encoding',
                            'GRE_T2_star' => 'GRE T2 star',
                            'BOLD_Retrieval' => 'BOLD Retrieval',
                            'ASL' => 'ASL',
        );
        if($_REQUEST['mri_type']) {
            $MRI_Type = $_REQUEST['mri_type'];
            $MRI_Type_Field = $_REQUEST['mri_type'] . "_Scan_Done";
            $MRIHeader = "$MRI_Type Breakdown";
            if($_REQUEST['mri_type'] == 'T1T2') {
                $MRI_Type_Field = "T1_Scan_Done='Complete' and T2_Scan_Done='Complete'";
                $MRIHeader = "T1 and T2 Complete Breakdown";
                $CaseStatement = "
                                CASE($MRI_Type_Field)
                                WHEN True then 'Complete'
                                WHEN False Then 'Incomplete'
                                ELSE 'Incomplete'
                                END";
                $MRISubcategories = array('Complete', 'Incomplete');
            } else {
                $CaseStatement = "
                                CASE($MRI_Type_Field)
                                WHEN 'Partial' THEN 'Partial Run'
                                WHEN 'No' THEN 'No Scan'
                                ELSE $MRI_Type_Field
                                END";
            }
        } else {
            $MRI_Type_Field = 'T1_Scan_Done';
            $MRI_Type = 'T1';
            $MRIHeader = "T1 Scan Done Breakdown";
            $CaseStatement = "
                            CASE($MRI_Type_Field)
                            WHEN 'Partial' THEN 'Partial Run'
                            WHEN 'No' THEN 'No Scan'
                            ELSE $MRI_Type_Field
                            END";
        }
        $scan_done = 'T1_Scan_done';
        $DB->select("SELECT s.SubprojectID,
                            c.CenterID,
                            CASE WHEN length(s.Visit_label) > 2 THEN SUBSTR(s.Visit_label, 4, 2)
                                 WHEN length(s.Visit_label) = 2 THEN s.Visit_label END as VLabel,
                            $CaseStatement as Subcat,
                            COUNT(*) as val
                     FROM mri_parameter_form m JOIN flag f USING (CommentID)
                                               JOIN session s ON (f.SessionID=s.ID)
                                               JOIN candidate c ON (c.CandID=s.CandID)
                     WHERE s.Current_stage <> 'Recycling Bin'
                           AND s.Active='Y'
                           AND c.Active='Y'
                           AND m.CommentID NOT LIKE 'DDE%'
                     GROUP BY Subcat, s.SubprojectID, c.CenterID, VLabel", &$result);
        $M_Visits = Utility::getVisitList();
        $this->tpl_data['MRI_Done_Table'] = $this->render_stats_table($MRIHeader, $MRISubcategories, $M_Visits, "mri_type", $MRIOptions, $MRI_Type, $centers, $result, "mri");

        foreach($result as $row) {
            $center = $row['CenterID'];
            $cohort = $row['SubprojectID'];
            $vl = $row['VLabel'];
            $c =& $this->tpl_data['mri_total'][$cohort]['C' . $center];

            $c[$vl] += $row['val'];
            $this->tpl_data['mri_total']['C' . $center][$vl]['total'] += $row['val'];
            $this->tpl_data['mri_total'][$cohort]['total'] += $row['val'];
            $this->tpl_data['mri_total']['total'] += $row['val'];

        }
        // MRI INTEGRITY STATS
        // MRI parameter form is completed, but nothing in tarchive
        /*
        $DB->select("SELECT COUNT(distinct f.CommentID) as val,
                            s.CenterID as CenterID
                            FROM flag f JOIN session s ON (f.SessionID=s.ID) 
                            LEFT JOIN mri_parameter_form m ON (m.CommentID=f.CommentID)
                            -- LEFT JOIN tarchive t ON (s.CandID=MID(t.PatientName, 9, 6))
                            LEFT JOIN tarchive t ON (s.CandID=MID(t.PatientName, 9, 6) AND MID(t.PatientName, 16, 3)=s.Visit_label)
                            LEFT JOIN candidate c ON (s.CandID=c.CandID)
                            WHERE f.CommentID NOT LIKE 'DDE%' 
                            AND f.Test_name='mri_parameter_form' 
                            and f.Administration <> 'None'
                            and t.TarchiveID IS NULL
                            AND s.Active='Y' 
                            AND t.TarchiveID IS NULL 
                            AND f.Data_entry='Complete'
                            AND f.CommentID NOT LIKE 'DDE%'
                            AND (m.T1_Scan_done IS NULL OR m.T1_Scan_done <> 'No')
                            AND s.SubprojectID IN ($subprojList)
                            GROUP BY s.CenterID;", $result);
        foreach($result as $row) {
            $this->tpl_data['mri_errors'][$row['CenterID']]['no_tarchive'] = $row['val'];
        }
         */

                    // MRI parameter form completed, but nothing in browser
						$result = $DB->pselect("SELECT s.CenterID as CenterID, 
								COUNT(distinct f.CommentID) as val
								FROM mri_parameter_form m LEFT JOIN flag f ON (f.CommentID=m.CommentID)
								LEFT JOIN session s ON (s.ID=f.SessionID) 
								LEFT JOIN files fi ON (fi.SessionID=f.SessionID) 
								WHERE (m.DTI64_Scan_done <> 'No' 
									OR m.T1_Scan_done <> 'No') 
								AND fi.FileID IS NULL 
								AND s.Active='Y' 
								AND s.CenterID <> 1
                                                                AND m.CommentID NOT LIKE 'DDE%' 
                                                                AND s.SubprojectID IN ($subprojList) 
								GROUP by s.CenterID"); //, $result);
						//OR m.T2_Scan_done <> 'No' 
						// AND f.Data_entry = 'Complete'
                    foreach($result as $row) {
                        $this->tpl_data['mri_errors'][$row['CenterID']]['no_browser'] = $row['val'];
                    }

        // MRI Browser populated, but no MRI parameter form completed.
        $DB->select("SELECT s.CenterID as CenterID, COUNT(distinct s.ID) as val
                            FROM files LEFT JOIN session s ON (files.SessionID=s.ID)
                            LEFT JOIN flag f on (f.SessionID=s.ID
                                AND f.Test_name='mri_parameter_form' 
                                AND f.CommentID NOT LIKE 'DDE%') 
                            LEFT JOIN mri_parameter_form mpf ON (mpf.CommentID=f.CommentID)
                            LEFT JOIN candidate c ON (c.CandID=s.CandID)
                            WHERE s.Active='Y'
                            AND (f.ID IS NULL OR f.Data_entry <> 'Complete' OR f.Data_entry IS NULL) 
                            AND c.PSCID <> 'scanner'
                            AND mpf.CommentID NOT LIKE 'DDE%'
                            AND s.SubprojectID IN ($subprojList)
                            $ExtraProject_Criteria
                            GROUP BY s.CenterID", $result);

                    foreach($result as $row) {
                        $this->tpl_data['mri_errors'][$row['CenterID']]['no_parameter'] = $row['val'];
                    }
                    // TOP MRI TABLE

                    // Below this has not been optimized. It's kind of ugly.
						foreach($centers as $center) {
							$id = $center['NumericID'];
							$name = $center['ShortName'];
							//$name = $center['LongName'];
							// -------- MRI STATS---------------

							$result24 = $DB->pselect("SELECT COUNT(s.CandID) FROM session as s, psc as p, candidate as c WHERE c.CandID=s.CandID AND s.Active='Y' AND s.CenterID=p.CenterID  AND s.Scan_done='Y' AND s.SubprojectID IN ($subprojList) AND s.CenterID={$id} ORDER BY s.CandID"); //, $result24);
							if (PEAR::isError($result24)) {
								return PEAR::raiseError("DB Error: ".$result24->getMessage());
							}
							list(,$result24)=each($result24[0]);
							$claimed_count = $result24;

							$result25 = $DB->pselect("SELECT COUNT(s.CandID) FROM mri_parameter_form as pf, session as s, flag as f, psc as p, candidate as c WHERE f.CommentID=pf.CommentID AND s.ID=f.SessionID AND c.CandID=s.CandID AND s.Active='Y' AND s.CenterID=p.CenterID AND pf.Data_entry_completion_status='Complete' AND s.CenterID={$id} AND s.SubprojectID IN ($subprojList) ORDER BY p.Name"); //, $result25);
							if (PEAR::isError($result25)) {
								return PEAR::raiseError("DB Error: ".$result25->getMessage());
							}
							list(,$result25)=each($result25[0]);
							$forms_count = $result25;
							$three_scans = $DB->pselectOne("SELECT count(distinct CandID) FROM session vEN join flag fEN ON (fEN.SessionID=vEN.ID and Test_name='mri_parameter_form' AND CommentID NOT LIKE 'DDE%') join mri_parameter_form mEN ON (fEN.CommentID=mEN.CommentID) 
									WHERE vEN.Active='Y' AND vEN.Visit_label LIKE '%EN%' and mEN.T1_Scan_Done='Complete'
									AND vEN.CenterID='{$id}' AND vEN.SubprojectID IN ($subprojList)");

							$mri_data[] = array('name' => $name,  'claimed_count' => $claimed_count, 'forms_count' => $forms_count, 'all_three_scans' => $three_scans);

							// AND EXISTS (SELECT 'x' FROM session v12 join flag f12 ON (f12.SessionID=v12.ID and Test_name='mri_parameter_form' AND CommentID NOT LIKE 'DDE%') join mri_parameter_form m12 ON (f12.CommentID=m12.CommentID)
							//               WHERE v12.Active='Y' AND v12.Cancelled='N' AND v12.Visit_label='v12' AND v06.CandID=v12.CandID AND m12.T1_Scan_Done='Complete') 
							//  AND EXISTS (SELECT 'x' FROM session v24 join flag f24 ON (f24.SessionID=v24.ID and Test_name='mri_parameter_form' AND CommentID NOT LIKE 'DDE%') join mri_parameter_form m24 ON (f24.CommentID=m24.CommentID)
							//                WHERE v24.Active='Y' AND v24.Cancelled='N' AND v24.Visit_label='v24' AND v06.CandID=v24.CandID and m24.T1_Scan_Done='Complete') 
						}
                    $subprojectList = "'" . implode("','", Utility::getSubprojectList()) . "'";

						// $MultiTimepoint = $DB->pselect("SELECT psc.PSCArea as Center, SUM(Three_T1_done) as T1, SUM(Three_DTI_Done) as DTI, SUM(Three_All_done) as Three_All_done FROM 
						//          (SELECT c.CenterID, COUNT(DISTINCT c.CandID)," .
						//SUM(Three_T2_Done) as T2 ,
						/* This essentially does COUNT(T1_Scan_done='Complete'), but because of the way COUNT(expr) works, 
						 * we need to make sure that anything that isn't 'Complete' is evaluated as NULL (not false), hence
						 * the crazy CASE statement. The different count statements Do v06, v06+v12, v06+v12+v24, all 3 for T2, all
						 * three for DTI, and all three for T1+T2+DTI. The outer select statement sums up all the groupings, to get
						 * a final scalar value.
						 */

						/*  "COUNT(DISTINCT CASE (COALESCE(mpf06.T1_Scan_done, 'No')='Complete') WHEN true THEN c.CandID ELSE null END) as v06_Done, 
						    FROM candidate c JOIN session v06 ON (c.CandID=v06.CandID AND v06.Visit_label LIKE '%EN%' and v06.Active='Y' and v06.Current_stage <> 'Recycling Bin') 
						    JOIN flag f06 ON (f06.SessionID=v06.ID and f06.Test_name='mri_parameter_form' AND f06.CommentID NOT LIKE 'DDE%') 
						    JOIN mri_parameter_form mpf06 ON (f06.CommentID=mpf06.CommentID) 
						    WHERE     c.Active='Y' AND c.PSCID  not like 'dcc%'  and c.PSCID <> 'scanner' 
						    GROUP BY c.CenterID) breakdown JOIN psc ON (breakdown.CenterID=psc.CenterID) WHERE psc.CenterID <> 1 GROUP BY psc.Name ORDER BY psc.CenterID", array()); */

						// COUNT(DISTINCT CASE (COALESCE(mpf12.T2_Scan_done, 'No')='Complete' AND COALESCE(mpf06.T2_Scan_done, 'No')='Complete' 
						//           AND COALESCE(mpf24.T2_Scan_done, 'No')='Complete') WHEN true THEN c.CandID ELSE null END) as Three_T2_done, 
						// AND COALESCE(mpf12.T2_Scan_done, 'No')='Complete' AND COALESCE(mpf06.T2_Scan_done, 'No')='Complete' 
						//  AND COALESCE(mpf24.T2_Scan_done, 'No')='Complete' 

						$scans = array('T1_Scan_done', 'FLAIR_Scan_done', 'DTI64_Scan_done', 'Fieldmap_DTI_Scan_done', 'Fieldmap_BOLD_Scan_done', 'BOLD_Resting_Scan_done', 'GRE_T2_star_Scan_done', 'ASL_Scan_done');
						$sum = 0;
						$sum_files = 0;
						$count = array();
						$total = array('Name' => 'Total', 'T1' => '0', 'FLAIR' => 0, 'DTI64' => 0, 'Fieldmap_DTI'=>0, 'Fieldmap_BOLD'=>0, 'BOLD_Resting'=>0, 'GRE_T2_star'=>0, 'ASL'=>0, 'All' => 0);
						$scans_inserted = array('Name' => 'Total', 'T1' => '0', 'FLAIR' => 0, 'DTI64' => 0, 'Fieldmap_DTI'=>0, 'Fieldmap_BOLD'=>0, 'BOLD_Resting'=>0, 'GRE_T2_star'=>0, 'ASL'=>0, 'All' => 0);
						$count_pass = array('Name' => 'Total', 'T1' => '0', 'FLAIR' => 0, 'DTI64' => 0, 'Fieldmap_DTI'=>0, 'Fieldmap_BOLD'=>0, 'BOLD_Resting'=>0, 'GRE_T2_star'=>0, 'ASL'=>0, 'All' => 0);
						$total_files = array('Name' => 'Total', 'T1' => '0', 'FLAIR' => 0, 'DTI64' => 0, 'Fieldmap_DTI'=>0, 'Fieldmap_BOLD'=>0, 'BOLD_Resting'=>0, 'GRE_T2_star'=>0, 'ASL'=>0, 'All' => 0);
						$total_pass = array('Name' => 'Total', 'T1' => '0', 'FLAIR' => 0, 'DTI64' => 0, 'Fieldmap_DTI'=>0, 'Fieldmap_BOLD'=>0, 'BOLD_Resting'=>0, 'GRE_T2_star'=>0, 'ASL'=>0, 'All' => 0);

						foreach (Utility::getSiteList() as $site) {
							$row = array(null=>"");
							if ($site != 'DCC') {
								$row['Center'] = $site;
								$sum = 0;
								$sum_files = 0;
								foreach ($scans as $s) {
									$type = preg_replace('/_Scan_done/', '', $s);
									$count = $DB->pselectRow("SELECT $s,  psc.Name as Center, COUNT(DISTINCT c.CandID) as count FROM candidate c 
											JOIN session s ON (c.CandID=s.CandID and s.Active='Y' and s.Current_stage <> 'Recycling Bin') 
											JOIN flag f ON (f.SessionID=s.ID and f.Test_name='mri_parameter_form' AND f.CommentID NOT LIKE 'DDE%') 
											JOIN mri_parameter_form m ON (f.CommentID=m.CommentID)  
											JOIN psc on c.CenterID = psc.CenterID
											WHERE     c.Active='Y' AND c.PSCID  not like 'dcc%'  
											AND c.PSCID <> 'scanner' 
											AND psc.Name = '$site' 
											AND m.$s = 'Complete' 
											GROUP BY c.CenterID ORDER BY c.CenterID");
									if (empty($count))
										$count['count'] = 0;
									$row[$type] = $count['count'];
									$sum += $row[$type];
									//print_r($count);
								}
								// 

								//foreach ($MultiTimepoint as $row) 
								$this->tpl_data['mri_all_scans_done'][] = array(
										'Name' => $row['Center'],
										'T1' => $row['T1'],
										'FLAIR' => $row['FLAIR'],
										'DTI64' => $row['DTI64'],
										'Fieldmap_DTI' => $row['Fieldmap_DTI'],
										'Fieldmap_BOLD' => $row['Fieldmap_BOLD'],
										'BOLD_Resting' => $row['BOLD_Resting'],
										'GRE_T2_star' => $row['GRE_T2_star'],
										'ASL' => $row['ASL'],
										'All' => $sum
										);
								$total['T1'] += $row['T1'];
								$total['FLAIR'] += $row['FLAIR'];
								$total['DTI64'] += $row['DTI64'];
								$total['Fieldmap_DTI'] += $row['Fieldmap_DTI'];
								$total['Fieldmap_BOLD'] += $row['Fieldmap_BOLD'];
								$total['BOLD_Resting'] += $row['BOLD_Resting'];
								$total['GRE_T2_star'] += $row['GRE_T2_star'];
								$toal['ASL'] += $row['ASL'];
								$total['All'] += $sum;

								/*                                $files = $DB->pselect("select m.Scan_type as type, f.File, count(*) as c, 
												  CASE WHEN length(s.Visit_label) > 2 THEN SUBSTR(s.Visit_label, 4, 2) 
												  WHEN length(s.Visit_label) = 2 THEN s.Visit_label END as VLabel
												  from files f join mri_scan_type m on m.ID = f.AcquisitionProtocolID 
												  join session s on f.sessionID = s.ID join psc on s.CenterID = psc.CenterID 
												  where s.Active = 'Y' and psc.Name = '$site' group by m.Scan_type");*/
								$files = $DB->pselect("select m.Scan_type as type, count(*) as c, 
										CASE WHEN length(s.Visit_label) > 2 THEN SUBSTR(s.Visit_label, 4, 2) 
										WHEN length(s.Visit_label) = 2 THEN s.Visit_label END as VLabel
										FROM files f 
										JOIN mri_scan_type m on m.ID = f.AcquisitionProtocolID 
										JOIN session s on f.sessionID = s.ID 
										JOIN psc on s.CenterID = psc.CenterID 
										WHERE s.Active = 'Y' and psc.Name = '$site' and s.Visit_label NOT LIKE '%phantom%' 
										GROUP by m.Scan_type, VLabel
										ORDER BY Scan_type");
								//print_r($files);

								$pass = $DB->pselect("select count(mriQCStatus) as count_pass, scan_type as type, 
										CASE WHEN length(s.Visit_label) > 2 THEN SUBSTR(s.Visit_label, 4, 2) 
										WHEN length(s.Visit_label) = 2 THEN s.Visit_label END as VLabel
										FROM psc p join session s using(CenterID) join files f on s.ID = f.SessionID 
										JOIN mri_scan_type m on f.AcquisitionProtocolID=m.ID 
										WHERE mriQCStatus = 'Pass' and p.Name = '$site' 
										GROUP BY scan_type, VLabel");
								/*$this->tpl_data['mri_all_scans_inserted'][] = array( 
								  'Name'  =>$site,
								  'T1'    =>100, 
								  'FLAIR' =>0,
								  'DTI64' =>0,
								  'Fieldmap_DTI'=>0, 
								  'Fieldmap_BOLD'=>0, 
								  'BOLD_Resting'=>0,   
								  'GRE_T2_star'=>0,                                    
								  'ASL'=>0                                    
								  );

								  $this->tpl_data['count_pass'][] = array( 
								  'Name'  =>$site,
								  'T1'    =>0, 
								  'FLAIR' =>0,
								  'DTI64' =>0,
								  'Fieldmap_DTI'=>0, 
								  'Fieldmap_BOLD'=>0, 
								  'BOLD_Resting'=>0,   
								  'GRE_T2_star'=>0,                                    
								  'ASL'=>0                                    
								  );*/

								$mappings = array('T1'=>'adniT1', 'FLAIR'=>'FLAIR', 'DTI64'=>'DTI', 'Fieldmap_BOLD'=>'Fieldmap', 'Fieldmap_DTI'=>'Fieldmap', 'BOLD_Resting'=>'Resting', 'GRE_T2_star'=>'greT2star', 'ASL'=>'ASL');
								//foreach($mappings as $k=>$v) {
								// } 
								foreach ($files as $file) {
									$scans_inserted['Name'] = $site;
									//$total_files[
									if ($file['type'] == 'adniT1') { 
										$scans_inserted['T1'] += $file['c'];
										$total_files['T1'] += $file['c'];
										$sum_files += $file['c'];
									}
									if ($file['type'] == 'FLAIR') {
										$scans_inserted['FLAIR'] = $file['c'];
										$total_files['FLAIR'] += $file['c'];
										$sum_files += $file['c'];
									}
									if ($file['type'] == 'DTI') {
										//  $this->tpl_data['mri_all_scans_inserted'][]['DTI64'] = $file['c'];
										$scans_inserted['DTI64'] = $file['c'];
										$total_files['DTI64'] += $file['c'];
										$sum_files += $file['c'];
									}
									if ($file['type'] == 'Fieldmap') {
											//$path = preg_replace('/\/data\/preventAD\/data\/assembly\/[0-9]+/', '', $file['File']);
											//$parts = explode('/', $path);
											//$visit_label = $parts[1];


										if ($file['VLabel'] == 'EN') {
											$scans_inserted['Fieldmap_DTI'] += $file['c'];
											$total_files['Fieldmap_DTI'] += $file['c'];
										} else {
											$scans_inserted['Fieldmap_BOLD'] += $file['c'];
											$total_files['Fieldmap_BOLD'] += $file['c'];
										}
										$sum_files += $file['c'];
									}   
									if ($file['type'] == 'Resting') {
										// $this->tpl_data['mri_all_scans_inserted'][]['BOLD_Resting'] = $file['c'];
										$scans_inserted['BOLD_Resting'] += $file['c'];
										$total_files['BOLD_Resting'] += $file['c'];
										$sum_files += $file['c'];
									}
									if ($file['type'] == 'greT2star') {
										// $this->tpl_data['mri_all_scans_inserted'][]['GRE_T2_star'] = $file['c'];
										$scans_inserted['GRE_T2_star'] += $file['c'];
										$total_files['GRE_T2_star'] += $file['c'];
										$sum_files += $file['c'];
									}
									if ($file['type'] == 'ASL') {
										// $this->tpl_data['mri_all_scans_inserted'][]['GRE_T2_star'] = $file['c'];
										$scans_inserted['ASL'] += $file['c'];
										$total_files['ASL'] += $file['c'];
										$sum_files += $file['c'];
									}
								}

								$sum_pass = 0;
								foreach ($pass as $p) {
									$count_pass['Name'] = $site;
									if ($p['type'] == 'adniT1') { 
										$count_pass['T1'] += $p['count_pass'];
										$total_pass['T1'] += $p['count_pass'];
										$sum_pass += $p['count_pass'];
									}
									if ($p['type'] == 'FLAIR') {
										$count_pass['FLAIR'] += $p['count_pass'];
										$total_pass['FLAIR'] += $p['count_pass'];
										$sum_pass += $p['count_pass'];
									}
									if ($p['type'] == 'DTI') {
										$count_pass['DTI64'] += $p['count_pass'];
										$total_pass['DTI64'] += $p['count_pass'];
										$sum_pass += $p['count_pass'];
									}
									if ($p['type'] == 'Fieldmap') {
										if ($p['VLabel'] == 'EN') {
											$count_pass['Fieldmap_DTI'] += $p['count_pass'];
											$total_pass['Fieldmap_DTI'] += $p['count_pass'];
										} else {
											$count_pass['Fieldmap_BOLD'] += $p['count_pass'];
											$total_pass['Fieldmap_BOLD'] += $p['count_pass'];
										}
										$sum_pass += $p['count_pass'];
									}   
									if ($p['type'] == 'Resting') {
										$count_pass['BOLD_Resting'] += $p['count_pass'];
										$total_pass['BOLD_Resting'] += $p['count_pass'];
										$sum_pass += $p['count_pass'];
									}
									if ($p['type'] == 'greT2star') {
										$count_pass['GRE_T2_star'] += $p['count_pass'];
										$total_pass['GRE_T2_star'] += $p['count_pass'];
										$sum_pass += $p['count_pass'];
									}
									if ($p['type'] == 'ASL') {
										$count_pass['ASL'] += $p['count_pass'];
										$total_pass['ASL'] += $p['count_pass'];
										$sum_pass += $p['count_pass'];
									}
								}
							}
						}
						$scans_inserted['All'] = $sum_files; 
						$count_pass['All'] = $sum_pass; 
						$total_files['All'] = $sum_files; 
						$total_pass['All'] = $sum_pass; 

						//                        $this->tpl_data['mri_all_scans_done'][] = $total;
						$this->tpl_data['mri_all_scans_inserted'][] = $scans_inserted;
						$this->tpl_data['count_pass'][] = $count_pass;

						$this->tpl_data['mri_total_scans_done'][] = $total;
						$this->tpl_data['mri_total_scans_inserted'] = $total_files;
						$this->tpl_data['total_pass'] = $total_pass;

						$this->tpl_data['mri_data'] = $mri_data;


						//MRI Feedback
						/*$feedback_comments = $DB->pselect("SELECT m.scan_type, fp.Comment, fi.File, count(f.PredefinedCommentID) as count 
						  from mri_scan_type m join files fi on m.ID = fi.AcquisitionProtocolID 
						  join feedback_mri_comments f on fi.FileID=f.FileID join feedback_mri_predefined_comments fp on f.PredefinedCommentID = fp.PredefinedCommentID 
						  WHERE f.SessionID IS NULL 
						  AND f.PredefinedCommentID = '4' OR f.PredefinedCommentID = '6' OR f.PredefinedCommentID = '8' OR f.PredefinedCommentID = '9' OR f.PredefinedCommentID = '36' 
						  GROUP BY f.PredefinedCommentID, m.scan_type");*/ 
						$feedback_comments = $DB->pselect("SELECT m.scan_type, fp.Comment, count(f.PredefinedCommentID) as count, 
								CASE WHEN length(s.Visit_label) > 2 THEN SUBSTR(s.Visit_label, 4, 2) 
								WHEN length(s.Visit_label) = 2 THEN s.Visit_label END as VLabel
								FROM mri_scan_type m join files fi on m.ID = fi.AcquisitionProtocolID 
								JOIN session s on fi.sessionID = s.ID
								JOIN feedback_mri_comments f on fi.FileID=f.FileID 
								JOIN feedback_mri_predefined_comments fp on f.PredefinedCommentID = fp.PredefinedCommentID 
								WHERE f.SessionID IS NULL 
								AND f.PredefinedCommentID = '4' OR f.PredefinedCommentID = '6' OR f.PredefinedCommentID = '8' OR f.PredefinedCommentID = '9' OR f.PredefinedCommentID = '36' 
								GROUP BY f.PredefinedCommentID, m.scan_type, VLabel"); 

							$comments = array();
						//print_r($feedback_comments);
						foreach ($feedback_comments as $f=>$g)
						{
							if (!array_key_exists($g['Comment'], $comments)) {
								$comments[$g['Comment']] = array('T1'=>0, 'FLAIR'=>0, '64 DTI'=>0, 'DTI Fieldmap'=>0, 'BOLD Fieldmap'=>0, 'Resting BOLD'=>0, 'GRE T2*'=>0, 'ASL'=>0, 'All'=>0);
							}
							if ($g['scan_type'] == 'adniT1') 
								$comments[$g['Comment']]['T1'] += $g['count'];
							if ($g['scan_type'] == 'DTI') 
								$comments[$g['Comment']]['64 DTI'] += $g['count'];
							if ($g['scan_type'] == 'FLAIR')
								$comments[$g['Comment']]['FLAIR'] += $g['count'];
							if ($g['scan_type'] == 'Fieldmap') {
								//	$path = preg_replace('/\/data\/preventAD\/data\/assembly\/[0-9]+/', '', $g['File']);
								if ($g['VLabel'] == 'EN')
									$comments[$g['Comment']]['DTI Fieldmap'] += $g['count'];
								else 
									$comments[$g['Comment']]['BOLD Fieldmap'] += $g['count'];
							}		
							if ($g['scan_type'] == 'Resting')  
								$comments[$g['Comment']]['Resting BOLD'] += $g['count'];
							if ($g['scan_type'] == 'greT2star')  
								$comments[$g['Comment']]['GRE T2*'] += $g['count'];
							if ($g['scan_type'] == 'ASL') 
								$comments[$g['Comment']]['ASL'] += $g['count'];
							//			else
							//				$comments[$g['Comment']][$g['scan_type']] += $g['count'];
							$comments[$g['Comment']]['All'] += $g['count'];

							if ($g['scan_type'] == 'adniT1') 
								$new_feedback['T1'][$g['Comment']] += $g['count'];
							elseif ($g['scan_type'] == 'DTI') 
								$new_feedback['64 DTI'][$g['Comment']] += $g['count'];
							elseif ($g['scan_type'] == 'Fieldmap') { 
								$new_feedback['DTI Fieldmap'][$g['Comment']] += $g['count'];
								$new_feedback['BOLD Fieldmap'][$g['Comment']] += $g['count'];
							}
							elseif ($g['scan_type'] == 'Resting')  
								$new_feedback['Resting BOLD'][$g['Comment']] += $g['count'];
							elseif ($g['scan_type'] == 'greT2star')  
								$new_feedback['GRE T2*'][$g['Comment']] += $g['count'];
							else
								$new_feedback[$g['scan_type']][$g['Comment']] += $g['count'];
							//foreach($g as $comment=>$count) {
							//$new_feedback[$g['Comment']] = $g['count'];
							//$this->tpl_data['mri_all_scans_done'][] = array(    
							//}
							foreach($mappings as $label=>$field) {
								if (!array_key_exists($new_feedback, $label))
									$new_feedback[$label] = 0;
							}

						}
						//print_r($new_feedback);
						//print_r($comments);
						$this->tpl_data['feedback_mri_comments'] = $comments;

	} // End function

    function stats_reliability()
    {
        $DB =& Database::singleton();
        $DB->select("SELECT CONCAT('C', CenterID) as ID, CenterID as NumericID, PSCArea as LongName, Name as ShortName FROM psc WHERE CenterID <> '1' AND Study_site = 'Y'", $centers);
        $this->tpl_data['Centers'] = $centers;
        $this->tpl_data['Sites'] = array('' => 'All Sites');
        $projects[null] = 'All Projects';
        foreach(Utility::getProjectList() as $key => $value) {
            $projects[$key] = $value;
        }
        $this->tpl_data['Projects'] = $projects;
        foreach ($centers as $row) {
            $this->tpl_data['Sites'][$row['NumericID']] = $row['LongName'];
            if($_REQUEST['ReliabilitySite'] == $row['NumericID']) {
                $this->tpl_data['CurrentSite'] = array('ID' => $row['NumericID'], 'Name' => $row['LongName']);
            }
        }
        if(isset($_REQUEST['ReliabilityProject']) && $_REQUEST['ReliabilityProject'] != '') {
            $ExtraProject_Criteria = ' AND (c.ProjectID IS NULL OR c.ProjectID=' . $_REQUEST['ReliabilityProject'] . ')';
            $currentProject = $_REQUEST['ReliabilityProject'];
            $this->tpl_data['CurrentProject'] = array('ID' => $currentProject, 'Name' => $projects[$currentProject]);
        } else {
            $ExtraProject_Criteria = '';
        }

        if($_REQUEST['ReliabilityProject'] == $row['NumericID']) {
            $this->tpl_data['CurrentProject'] = array('ID' => $row['NumericID'], 'Name' => $row['LongName']);
        }
        $this->tpl_data['reliability_completion'] = $this->getReliabilityData();
    }

    function stats_demographic()
    {
        $DB =& Database::singleton();

        $projects[null] = 'All Projects';
        foreach(Utility::getProjectList() as $key => $value) {
            $projects[$key] = $value;
        }
        if(isset($_REQUEST['DemographicSite']) && $_REQUEST['DemographicSite'] != '') {
            $ExtraSite_Criteria = 'AND (c.CenterID IS NULL OR c.CenterID=' . $_REQUEST['DemographicSite'] . ')';
        }
        else {
            $ExtraSite_Criteria = '';
        }
        if(isset($_REQUEST['DemographicProject']) && $_REQUEST['DemographicProject'] != '') {
            $ExtraProject_Criteria .= ' AND c.ProjectID=' . $_REQUEST['DemographicProject'];
            $currentProject = $_REQUEST['DemographicProject'];
            $this->tpl_data['CurrentProject'] = array('ID' => $currentProject, 'Name' => $projects[$currentProject]);
        }
        $subprojects = Utility::getSubprojectsForProject($currentProject);
        foreach($subprojects as $key=>$val) {
            $subprojList .= $key.",";
        }
        $subprojList = substr($subprojList, 0, -1);

        $demographics_project_name = $projects["{$_REQUEST['DemographicProject']}"];
        $this->tpl_data['Subprojects'] = $subprojects;
        $this->tpl_data['Projects'] = $projects;
        $DB->select("SELECT s.subprojectid as rowid, 
            COUNT(DISTINCT(c.PSCID)) as val 
            FROM candidate as c LEFT JOIN session s ON (s.CandID=c.CandID) 
            WHERE s.active='Y' AND s.CenterID <> '1'        
            AND c.Active='Y'
            AND (s.Current_stage IN ('Visit', 'Screening', 'Approval')) 
            AND COALESCE(s.Screening,'') NOT IN ('Failure', 'Withdrawal')
            AND COALESCE(s.Visit,'') NOT IN ('Failure', 'Withdrawal')
            $ExtraSite_Criteria $ExtraProject_Criteria
            GROUP BY s.subprojectid", $result);
        foreach($result as $row) {
            $this->tpl_data['registered'][$row['rowid']]['visit'] = $row['val'];
            $this->tpl_data['registered']['visittotal'] += $row['val'];

            // Total, explicitly calculated instead of added in the loop to ensure there's no one double registered in 2 different cohorts
            $DB->select("SELECT count(distinct(c.PSCID)) from candidate as c WHERE c.PSCID<>'scanner' AND c.CenterID<>'1' AND c.Active='Y' $ExtraSite_Criteria $ExtraProject_Criteria", $total);
            list(,$total)=each($total[0]);
            $Total_candidates = $total;
            $this->tpl_data['Total_candidates'] = $Total_candidates;

        }
        // Total for 'registered candidates' row.
        $DB->select("SELECT s.subprojectid as rowid, count(DISTINCT(c.PSCID)) as val from candidate as c left join session s ON (s.CandID=c.CandID) WHERE coalesce(s.active, 'Y')='Y' AND c.CenterID <> '1' and c.PSCID <> 'scanner' and coalesce(s.visit_label, 'v06') in ('v06', 'v12', 'v18', 'v24', 'v36') AND c.Active='Y' $ExtraSite_Criteria $ExtraProject_Criteria group by s.subprojectid", $result);
        foreach($result as $row) {
            $this->tpl_data['registered'][$row['rowid']]['total'] = $row['val'];
            $this->tpl_data['registered']['total'] += $row['val'];
        }

        // Number of visits.
        // This is based on the EDI completion status, since the EDI
        // is done by every cohort at every visit
        $DB->select("SELECT count(DISTINCT c.PSCID) as val, s.SubprojectID as rowid FROM flag f join session s ON (s.ID=f.SessionID) join candidate c ON (c.CandID=s.CandID)  where f.Test_name IN
                ('edi', 'mri_parameter_form', 'vineland_subject')
                AND COALESCE(s.Screening,'') NOT IN ('Failure', 'Withdrawal')
                AND COALESCE(s.Visit,'') NOT IN ('Failure', 'Withdrawal')
                AND s.Active='Y'
                AND c.Active='Y'
                AND (f.Administration = 'All' OR f.Administration='Partial')
                AND s.Current_stage <> 'Recycling Bin'
                $ExtraSite_Criteria $ExtraProject_Criteria
                GROUP BY s.SubprojectID", $result);
        foreach($result as $row) {
            $this->tpl_data['edi'][$row['rowid']]['complete'] = $row['val'];
            $this->tpl_data['edi']['complete'] += $row['val'];
        }

        // Number of T1 candidates scanned
        $DB->select("SELECT count(DISTINCT c.PSCID) as val, s.SubprojectID as rowid FROM flag f JOIN mri_parameter_form mp USING(CommentID) join session s ON (s.ID=f.SessionID) join candidate c ON (c.CandID=s.CandID) WHERE f.Test_name='mri_parameter_form'
                AND COALESCE(s.Screening,'') NOT IN ('Failure', 'Withdrawal')
                AND COALESCE(s.Visit,'') NOT IN ('Failure', 'Withdrawal')
                AND s.Active='Y'
                AND c.Active='Y'
                AND f.Administration <> 'None'
                AND mp.T1_Scan_done='Complete'
                AND s.Current_stage <> 'Recycling Bin'
                $ExtraSite_Criteria $ExtraProject_Criteria
                GROUP BY s.SubprojectID", $result);
        foreach($result as $row) {
            $this->tpl_data['scanned'][$row['rowid']]['complete'] = $row['val'];
            $this->tpl_data['scanned']['complete'] += $row['val'];
        }


        // Adds a C to the start of the centerid, because that's how the
        // template is looking for it. Exclude DCC 
        $DB->select("SELECT CONCAT('C', CenterID) as ID, CenterID as NumericID, PSCArea as LongName, Name as ShortName FROM psc WHERE CenterID <> '1' AND Study_site = 'Y'", $centers);
        $this->tpl_data['Centers'] = $centers;
        $this->tpl_data['Sites'] = array('' => 'All Sites');
        foreach ($centers as $row) {
            $this->tpl_data['Sites'][$row['NumericID']] = $row['LongName'];
            if($_REQUEST['DemographicSite'] == $row['NumericID']) {
                $this->tpl_data['CurrentSite'] = array('ID' => $row['NumericID'], 'Name' => $row['LongName']);
            }

        }
        $Visits = Utility::getExistingVisitLabels($currentProject);
        $this->tpl_data['Visits'] = $Visits;

        $instruments  = array_merge(
            array( '' => 'Recruit Gender Breakdown'),
            Utility::getAllInstruments()
        );
        //$this->tpl_data['scored_instruments'] = Utility::getScoredInstruments();
        $this->tpl_data['all_instruments'] = Utility::getAllInstruments();
        if(isset($_REQUEST['DemographicInstrument']) && $_REQUEST['DemographicInstrument'] != '') {
            $Subcategories = array('Complete', 'Incomplete');
            $DB->select("SELECT count(*) as val, 
                f.Data_entry as Subcat, 
                c.CenterID as CenterID, 
                s.SubprojectID as SubprojectID,
                s.visit_label as VLabel
                FROM session s JOIN candidate c ON (s.CandID=c.CandID)
                JOIN flag f ON (f.SessionID=s.ID)
                JOIN $_REQUEST[DemographicInstrument] i USING(CommentID)
                WHERE s.Active='Y' AND
                c.CenterID <> '1' AND f.Data_entry='Complete' AND
                f.Administration='All' AND
                f.CommentID NOT LIKE 'DDE%'
                $ExtraProject_Criteria
                GROUP BY c.CenterID, SubprojectID, VLabel, Subcat
                UNION
                SELECT count(*) as val, 'Incomplete' as Subcat, 
                c.CenterID as CenterID, 
                s.SubprojectID as SubprojectID,
                s.visit_label as VLabel
                FROM session s JOIN candidate c USING(CandID) 
                JOIN flag f ON (f.SessionID=s.ID) 
                JOIN $_REQUEST[DemographicInstrument] i USING(CommentID)
                WHERE s.Active='Y' and s.CenterID <> 1
                $ExtraProject_Criteria
                AND f.CommentID NOT LIKE 'DDE%' AND (f.Data_entry IS NULL OR f.Data_entry <> 'Complete')
                GROUP BY c.CenterID, SubprojectID, VLabel, Subcat
                ", $result);
            $this->tpl_data['RecruitsTable'] = $this->render_stats_table("Data Entry Completion Status for $_REQUEST[DemographicInstrument]", $Subcategories, $Visits, "DemographicInstrument", $instruments, $_REQUEST['DemographicInstrument'], $centers, $result, "demographics",$currentProject);
        } else {
            $Subcategories = array('Male', 'Female');
            $DB->select("SELECT c.CenterID as CenterID, 
                s.SubprojectID as SubprojectID, 
                s.visit_label as VLabel,
                c.gender as Subcat,
                count(s.CandID) as val
                FROM session s JOIN candidate c ON (s.CandID=c.CandID) 
                WHERE s.active='Y' AND s.CenterID <> '1'
                AND (s.Current_stage IN ('Visit', 'Screening', 'Approval')
                AND s.SubprojectID IN ($subprojList))  
                AND COALESCE(s.Screening,'') NOT IN ('Failure', 'Withdrawal')
                AND COALESCE(s.Visit,'') NOT IN ('Failure', 'Withdrawal')
                GROUP BY c.CenterID, SubprojectID, VLabel, Subcat", $result);
            $this->tpl_data['RecruitsTable'] = $this->render_stats_table("Breakdown of Registered Candidates", $Subcategories, $Visits, "DemographicInstrument", $instruments, '', $centers, $result, "demographics", $currentProject);

        }
    }



    static function getEnrollmentData($siteID= null, $projID= null){
        $DB =& Database::singleton();
        $enroll_data = array();
        $params = array();
        $Extra_Where = '';
        if(!empty($siteID)) {
            $Extra_Where .= " AND sn.CenterID = :site"; //$_REQUEST['site'];
            $params['site'] = $siteID;
        }
        if(!empty($projID)) {
            $Extra_Where .= " AND  cad.ProjectID = :project"; //$_REQUEST['site'];
            $params['project'] = $projID;
        }
        $ethnic_categories = array("hispanic","non_hispanic","not_answered");
        $ethnic_name = array("Hispanic or Latino", "Not Hispanic or Latino",
            "Unknown (individuals not reporting ethnicity");
        $cat_count = 0;
        $cat_total = array('Female' =>0, 'Male' =>0, 'Unknown' =>0);
        foreach($ethnic_categories as $ethnic_cat){
            $gender = array('Female' => 0, 'Male' => 0, 'Unknown' => 0);
            $results = $DB->pselect("SELECT ProjectID, COALESCE(Gender, 'Unknown') as Gender, COUNT( DISTINCT cad.CandID) as val FROM candidate cad 
                    JOIN session sn on cad.CandID= sn.CandID 
                    JOIN flag fg on sn.ID = fg.SessionID
                    JOIN tsi ti on fg.CommentID = ti.CommentID 
                    WHERE ti.child_ethnicity = '$ethnic_cat' 
                    AND fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' 
                    AND cad.Active='Y' AND fg.Data_Entry='Complete' $Extra_Where AND fg.Administration <> 'None' GROUP BY Gender", $params);
            if(is_null($results)){
            }else{
                foreach($results as $row){
                    $Gender = $row['Gender'];
                    $Count = $row['val'];
                    //$total_count += $Count;
                    if(is_null($Gender)) {
                        $gender['Unknown'] = $Count;
                    } else {
                        $gender[$Gender] = $Count;
                        $cat_total[$Gender] += $Count;
                    }

                }}
            $enroll_data[] = array('ethnic_cat' => $ethnic_name[$cat_count],'female_count' => $gender['Female'],'male_count' => $gender['Male'],'unknown_count' => $gender['Unknown'],'total_count' =>array_sum($gender) );
            $cat_count++;
        }
        $enroll_data_total[] = array('total_name' =>" Ethnic Category: Total of All Subjects",'total_female'=>$cat_total['Female'],'total_male'=>$cat_total['Male'],'total_unknown'=> $cat_total['Unknown'],'total_total'=> array_sum($cat_total));

        $enroll_data_race = array();

        $race_categories = array("american_indian_alaska_native","asian","native_hawaiian_pacific_islander","black_african_american","white","more_than_one_race","unknown_not_reported");
        $race_name = array("American Indian/Alaska Native", "Asian", "Native Hawaiian or Other Pacific Islander","Black or African American","White"," More Than One Race","Unknown or Not Reported");
        $race_count = 0;
        $race_total = array('Female' =>0, 'Male' =>0, 'Unknown' =>0);
        $gender = array('Female' => 0, 'Male' => 0, 'Unknown' => 0);

        foreach($race_categories as $race_cat){
            $gender = array('Female' => 0, 'Male' => 0, 'Unknown' => 0);

            if($race_cat === "more_than_one_race"){
                $race_cat = '%'.$race_cat.'%';
                $results = $DB->pselect("SELECT COALESCE(Gender, 'Unknown') as Gender, COUNT( DISTINCT cad.CandID) as val FROM candidate cad
                        JOIN session sn on cad.CandID= sn.CandID
                        LEFT JOIN flag fg on sn.ID = fg.SessionID
                        LEFT JOIN tsi ti on fg.CommentID = ti.CommentID WHERE ( ti.candidate_race LIKE  '$race_cat' OR ti.candidate_race LIKE '%{@}%' ) AND fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' AND cad.Active='Y' AND fg.Data_Entry='Complete' $Extra_Where AND fg.Administration <> 'None' AND fg.Test_name='tsi' GROUP BY Gender" , $params);
            }
            else if($race_cat === "unknown_not_reported"){
                $results = $DB->pselect("SELECT COALESCE(Gender, 'Unknown') as Gender, COUNT( DISTINCT cad.CandID) as val FROM candidate cad
                        JOIN session sn on cad.CandID= sn.CandID
                        LEFT JOIN flag fg on sn.ID = fg.SessionID
                        LEFT JOIN tsi ti on fg.CommentID = ti.CommentID WHERE (ti.candidate_race LIKE  '$race_cat'OR ti.candidate_race IS NULL) AND fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' AND cad.Active='Y' AND fg.Data_Entry='Complete' $Extra_Where AND fg.Administration <> 'None' AND fg.Test_name='tsi' GROUP BY Gender" ,$params);

            }else{

                $results = $DB->pselect("SELECT COALESCE(Gender, 'Unknown') as Gender, COUNT( DISTINCT cad.CandID) as val FROM candidate cad
                        JOIN session sn on cad.CandID= sn.CandID
                        LEFT JOIN flag fg on sn.ID = fg.SessionID
                        LEFT JOIN tsi ti on fg.CommentID = ti.CommentID WHERE ti.candidate_race LIKE  '$race_cat' AND fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' AND cad.Active='Y' AND fg.Data_Entry='Complete' $Extra_Where AND fg.Administration <> 'None' AND fg.Test_name='tsi' GROUP BY Gender" , $params);
            }

            $Count = 0;
            foreach($results as $row){
                $Gender = $row['Gender'];
                $Count = $row['val'];
                if(is_null($Count)){
                    $Count = 0;
                }
                //$total_count += $Count;
                if(is_null($Gender)) {
                    $gender['Unknown'] = $Count;
                } else {
                    $gender[$Gender] = $Count;
                    $race_total[$Gender] += $Count;
                }

            }


            $enroll_data_race[] = array('race_cat' => $race_name[$race_count],'female_count' => $gender['Female'],'male_count' => $gender['Male'],'unknown_count' => $gender['Unknown'],'total_count' => array_sum($gender));
            $race_count++;

        }
        $enroll_data_racetotal[] = array('total_name' =>" Racial Categories: Total of All Subjects",'total_female'=>$race_total['Female'],'total_male'=>$race_total['Male'],'total_unknown'=> $race_total['Unknown'],'total_total'=> array_sum($race_total));


        $enroll_data_hispanic = array();
        $hispanic_race_count = 0;
        $hispanic_total = array('Female' =>0, 'Male' =>0, 'Unknown' =>0);
        $gender = array('Female' => 0, 'Male' => 0, 'Unknown' => 0);

        foreach($race_categories as $race_cat){
            $gender = array('Female' => 0, 'Male' => 0, 'Unknown' => 0);
            if($race_cat === "more_than_one_race"){
                $race_cat = '%'.$race_cat.'%';
                $results = $DB->pselect("SELECT COALESCE(Gender, 'Unknown') as Gender, COUNT( DISTINCT cad.CandID) as val FROM candidate cad
                        JOIN session sn on cad.CandID= sn.CandID
                        LEFT JOIN flag fg on sn.ID = fg.SessionID
                        LEFT JOIN tsi ti on fg.CommentID = ti.CommentID WHERE ( ti.candidate_race LIKE  '$race_cat' OR ti.candidate_race LIKE '%{@}%' ) AND ti.child_ethnicity='hispanic' AND fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' AND cad.Active='Y' AND fg.Data_Entry='Complete' $Extra_Where AND fg.Administration <> 'None' AND fg.Test_name='tsi' GROUP BY Gender" , $params);
            }
            else if($race_cat === "unknown_not_reported"){
                $results = $DB->pselect("SELECT COALESCE(Gender,'Unknown') as Gender ,COUNT( DISTINCT cad.CandID) as val FROM candidate cad
                        JOIN session sn on cad.CandID= sn.CandID
                        LEFT JOIN flag fg on sn.ID = fg.SessionID
                        LEFT JOIN tsi ti on fg.CommentID = ti.CommentID WHERE (ti.candidate_race LIKE '$race_cat' OR ti.candidate_race IS NULL) AND ti.child_ethnicity='hispanic' AND fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' AND cad.Active='Y' AND fg.Data_Entry='Complete' $Extra_Where  AND fg.Administration <> 'None' AND fg.Test_name='tsi' GROUP BY Gender" , $params);
            }else{
                $results = $DB->pselect("SELECT COALESCE(Gender, 'Unknown') as Gender,COUNT( DISTINCT cad.CandID) as val FROM candidate cad
                        JOIN session sn on cad.CandID= sn.CandID
                        LEFT JOIN flag fg on sn.ID = fg.SessionID
                        LEFT JOIN tsi ti on fg.CommentID = ti.CommentID WHERE ti.candidate_race LIKE '$race_cat' AND ti.child_ethnicity='hispanic' AND fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' AND cad.Active='Y' AND fg.Data_Entry='Complete'  $Extra_Where  AND fg.Administration <> 'None' AND fg.Test_name='tsi' GROUP BY Gender" , $params);
            }

            foreach($results as $row){
                $Gender = $row['Gender'];
                $Count = $row['val'];
                if(is_null($Count)){
                    $Count = 0;
                }
                //$total_count += $Count;
                $gender[$Gender] = $Count;
                $hispanic_total[$Gender] += $Count;


            }

            $enroll_data_hispanic[] = array('race_cat' => $race_name[$hispanic_race_count],'female_count' => $gender['Female'],'male_count' => $gender['Male'],'unknown_count' => $gender['Unknown'],'total_count' => array_sum($gender));
            $hispanic_race_count++;

        }
        $enroll_data_hispanictotal[] = array('total_name' =>" Racial Categories: Total of Hispanic or Latinos",'total_female'=>$hispanic_total['Female'],'total_male'=>$hispanic_total['Male'],'total_unknown'=> $hispanic_total['Unknown'],'total_total'=> array_sum($hispanic_total));

        $withdraw = $DB->pselectOne("SELECT COUNT( DISTINCT cad.CandID) as val FROM candidate cad
                JOIN session sn on cad.CandID= sn.CandID
                LEFT JOIN flag fg on sn.ID = fg.SessionID
                LEFT JOIN tsi ti on fg.CommentID = ti.CommentID WHERE fg.CommentID NOT LIKE 'DDE%' AND sn.CenterID <> '1' AND sn.Active='Y' AND cad.Active='Y' AND fg.Data_Entry='Complete'  $Extra_Where  AND fg.Administration <> 'None' AND (sn.Screening IN ('Failure','Withdrawal') OR sn.Visit IN ('Withdrawal','Failure') ) " , $params);


        $enroll_results = array('enroll_data'=> $enroll_data,'enroll_data_total'=> $enroll_data_total,'enroll_data_race'=> $enroll_data_race,'enroll_data_racetotal'=>$enroll_data_racetotal,'enroll_data_hispanic'=>$enroll_data_hispanic,'enroll_data_hispanictotal'=> $enroll_data_hispanictotal , 'withdraw' => $withdraw);

        return $enroll_results;

    }



    function _getCrossSiteReliabilityData($name, $inst_name, $cutoff, $extra_where="") {
        $data = $this->_getSingleReliabilityData($name, $inst_name, $cutoff,
            " $extra_where AND CommentID NOT LIKE CONCAT('%', CASE reliability_center_id WHEN 1 THEN 'dcc' WHEN 2 THEN 'SEA' WHEN 3 THEN 'PHI' WHEN 4 THEN 'STL' WHEN 5 THEN 'UNC' WHEN 6 THEN 'UofA' END, '%')");
        $data['name'] = $name . ' (Cross Site)';
        return $data;
    }
    function _getWithinSiteReliabilityData($name, $inst_name, $cutoff, $extra_where = "") {
        $data = $this->_getSingleReliabilityData($name, $inst_name, $cutoff,
            " $extra_where AND CommentID like CONCAT('%', CASE reliability_center_id WHEN 1 THEN 'dcc' WHEN 2 THEN 'SEA' WHEN 3 THEN 'PHI' WHEN 4 THEN 'STL' WHEN 5 THEN 'UNC' WHEN 6 THEN 'UofA' END, '%')");
        $data['name'] = $name . ' (Within Site)';
        return $data;
    }
    function _getSingleReliabilityData($name, $inst_name, $cutoff, $extra_where = '') {
        if(isset($_REQUEST['ReliabilitySite']) && $_REQUEST['ReliabilitySite'] != '') {
            $extra_where .= ' AND c.CenterID=' . $_REQUEST['ReliabilitySite'];
        }
        if(isset($_REQUEST['ReliabilityProject']) && $_REQUEST['ReliabilityProject'] != '') {
            $ExtraProject_Criteria = ' AND (c.ProjectID IS NULL OR c.ProjectID=' . $_REQUEST['ReliabilityProject'] . ')';
        } else {
            $ExtraProject_Criteria = '';
        }

        $db = Database::singleton();
        $ret = array();
        $ret['name'] = $name;
        $ret['complete'] = $db->selectOne("SELECT count(*) from reliability r JOIN flag f USING(CommentID) JOIN session s ON (s.ID=f.SessionID) JOIN candidate c ON (c.CandID=s.CandID) where Instrument='$inst_name' and Invalid <> 'yes' and Reliability_score IS NOT NULL AND c.CenterID <> 1 $extra_where $ExtraProject_Criteria");
        $ret['reliable'] = $db->selectOne("SELECT count(*) from reliability r JOIN flag f USING(CommentID) JOIN session s ON (s.ID=f.SessionID) JOIN candidate c ON (c.CandID=s.CandID)where Instrument='$inst_name' and Invalid <> 'yes' and Reliability_score IS NOT NULL and Reliability_score > $cutoff AND c.CenterID <> 1 $extra_where $ExtraProject_Criteria");
        $ret['total'] = $db->selectOne("SELECT count(*) from reliability r JOIN flag f USING(CommentID) JOIN session s ON (s.ID=f.SessionID) JOIN candidate c ON (c.CandID=s.CandID) where Instrument='$inst_name' and Invalid <> 'yes' AND c.CenterID <> 1 $extra_where $ExtraProject_Criteria");
        if($ret['total'] == 0) {
            // These should probably be indeterminate instead of 0, but 0
            // looks cleaner.
            $ret['percent_complete'] = '0';
            $ret['percent_reliable'] = '0';
        } else {
            $ret['percent_complete'] = round($ret['complete'] / $ret['total'] * 100);
            $ret['percent_reliable'] = round($ret['reliable'] / $ret['total'] * 100);
        }
        return $ret;
    }

    function getReliabilityData() {
        // Whether something is reliable or not isn't stored in the database,
        // but calculated on the fly. 
        // As a result, we need to manually calculate it and maintain for each
        // type of thing Tanya wants stats for here separately

        // Thresholds was copy/pasted from 
        require_once __DIR__ . "/../../reliability/php/NDB_Menu_Filter_reliability.class.inc";
        $thresholds = NDB_Menu_Filter_Reliability::getThreshholds();

        $tests = array(
            'adi_r_proband' => 'ADI-R (Proband)',
            'adi_r_subject' => 'ADI-R (Subject)',
            'aosi' => 'AOSI',
            'ados_module1' => 'ADOS (Module 1)',
            'ados_module2' => 'ADOS (Module 2)',
            'csbs' => 'CSBS'

        );
        $within_cross = array('adi_r_subject', 'ados_module1', 'ados_module2', 'aosi', 'csbs');

        $reliable = array();
        $within_reliable = array();
        $across_reliable = array();
        foreach($tests as $inst => $name) {
            if(in_array($inst, $within_cross)) {
                if($inst == 'aosi') {
                    $within_reliable[] = $this->_getWithinSiteReliabilityData(
                        $name . " v06",
                        $inst, $thresholds[$inst],
                        " AND EXISTS (SELECT 'x' FROM flag f JOIN session s ON (s.ID=f.SessionID) WHERE f.CommentID=r.CommentID and lower(s.Visit_label)='v06')");
                    $across_reliable[] = $this->_getCrossSiteReliabilityData(
                        $name . " v06",
                        $inst, $thresholds[$inst],
                        " AND EXISTS (SELECT 'x' FROM flag f JOIN session s ON (s.ID=f.SessionID) WHERE f.CommentID=r.CommentID and lower(s.Visit_label)='v06')");
                    $within_reliable[] = $this->_getWithinSiteReliabilityData(
                        $name . " v12",
                        $inst, $thresholds[$inst],
                        " AND EXISTS (SELECT 'x' FROM flag f JOIN session s ON (s.ID=f.SessionID) WHERE f.CommentID=r.CommentID and lower(s.Visit_label)='v12')");
                    $across_reliable[] = $this->_getCrossSiteReliabilityData(
                        $name . " v12",
                        $inst, $thresholds[$inst],
                        " AND EXISTS (SELECT 'x' FROM flag f JOIN session s ON (s.ID=f.SessionID) WHERE f.CommentID=r.CommentID and lower(s.Visit_label)='v12')");
                } else {
                    $within_reliable[] = $this->_getWithinSiteReliabilityData($name,
                        $inst, $thresholds[$inst]);
                    $across_reliable[] = $this->_getCrossSiteReliabilityData($name,
                        $inst, $thresholds[$inst]);
                }
            } else {
                $within_reliable[] = $this->_getSingleReliabilityData($name,
                    $inst, $thresholds[$inst]);
            }
        }

        return array_merge($across_reliable, $within_reliable);
    }
} // End class
?>
