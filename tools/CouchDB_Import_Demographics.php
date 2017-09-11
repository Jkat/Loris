<?php
require_once 'generic_includes.php';
require_once 'CouchDB.class.inc';
require_once 'Database.class.inc';
class CouchDBDemographicsImporter {
    var $SQLDB; // reference to the database handler, store here instead
                // of using Database::singleton in case it's a mock.
    var $CouchDB; // reference to the CouchDB database handler

    // this is just in an instance variable to make
    // the code a little more readable.
    var $Dictionary = array(
        'DoB' => array(
            'Description' => 'Date of Birth',
            'Type' => 'varchar(255)'
        ),
        'CandID' => array(
            'Description' => 'DCC Candidate Identifier',
            'Type' => 'varchar(255)'
        ),
        'PSCID' => array(
            'Description' => 'Project Candidate Identifier',
            'Type' => 'varchar(255)'
        ),
        'Visit_label' => array(
            'Description' => 'Visit of Candidate',
            'Type' => 'varchar(255)'
        ),
        'Cohort' => array(
            'Description' => 'Cohort of this session',
            'Type' => 'varchar(255)'
        ),
        'Gender' => array(
            'Description' => 'Candidate\'s gender',
            'Type' => "enum('Male', 'Female')"
        ),
        'DoB' => array(
            'Description' => 'Candidate\'s date of birth',
            'Type' => "date"
        ),
        'Mother_tongue' => array(
            'Description' => 'Candidate\'s mother tongue',
            'Type' => "enum('Arabic','Chinese','Creole','English','Greek','French','Italian','Portuguese','Romanian','Spanish','Other','Unknown')"
        ),
        'Site' => array(
            'Description' => 'Site that this visit took place at',
            'Type' => "varchar(3)",
        ),
        'Current_stage' => array(
            'Description' => 'Current stage of visit',
            'Type' => "enum('Not Started','Screening','Visit','Approval','Subject','Recycling Bin')"
        ),  
        'Visit' => array(
            'Description' => 'Visit status',
            'Type' => "enum('Pass','Failure','Withdrawal','In Progress')"
        ),
        'Failure' =>  array(
            'Description' => 'Whether Recycling Bin Candidate was failure or withdrawal',
            'Type' => "enum('Failure','Withdrawal','Neither')",
        ),
	'Status' => array(
	    'Description' => 'Participant status',
	    'Type' => "varchar(255)"
	),
        'entry_staff' => array(
            'Description' => 'Participant status entry staff',
            'Type' => "varchar(255)"
        ),
        'data_changed_date' => array(
            'Description' => 'Participant status data changed date',
            'Type' => "date"
        ),
        'data_entry_date' => array(
            'Description' => 'Participant status data entry date',
            'Type' => "date"
        ),
	'reason_specify' => array(
	    'Description' => 'Participant status reason',
	    'Type' => "text"
	),
        'reason_specify_status' => array(
            'Description' => 'Participant status reason status',
            'Type' => "enum('dnk','not_applicable','refusal','not_answered')"
        ),
        'withdrawal_reasons' => array(
            'Description' => 'Participant status withdrawal reason',
            'Type' => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')"
        ),
	'withdrawal_reasons_other_specify' => array(
	    'Description' => 'Other reason for withdrawal',
	    'Type' => "text"
	),
        'withdrawal_reasons_other_specify_status' => array(
            'Description' => 'Other reason for withdrawal status',
            'Type' => "enum('dnk','not_applicable','refusal','not_answered')"
        ),
        'naproxen_eligibility' => array(
            'Description' => 'Naproxen Eligibility',
            'Type' => "enum('yes','no')"
        ),
        'naproxen_eligibility_status' => array(
            'Description' => 'Naproxen Eligibility Status',
            'Type' => "enum('active','stop_medication_active','withdrawn','excluded','death','completed','stop_medication_completed')"
        ),
        'naproxen_eligibility_reason_specify' => array(
            'Description' => 'Naproxen Eligibility Reason',
            'Type' => "text"
        ),
        'naproxen_eligibility_reason_specify_status' => array(
            'Description' => 'Naproxen Eligibility Reason status',
            'Type' => "enum('not_answered')"
        ),
        'naproxen_excluded_reason_specify' => array(
            'Description' => 'Naproxen Excluded Reason',
            'Type' => "text"
        ),
        'naproxen_excluded_reason_specify_status' => array(
            'Description' => 'Naproxen Excluded Reason status',
            'Type' => "enum('not_answered')"
        ),
        'naproxen_withdrawal_reasons' => array(
            'Description' => 'Naproxen Withdrawal Reason',
            'Type' => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')"
        ),
        'naproxen_withdrawal_reasons_other_specify' => array(
            'Description' => 'Naproxen Withdrawal Reason Other',
            'Type' => "text"
        ),
        'naproxen_withdrawal_reasons_other_specify_status' => array(
            'Description' => 'Naproxen Withdrawal Reason Other status',
            'Type' => "enum('not_answered')"
        ),
        'probucol_eligibility' => array(
            'Description' => 'Probucol Eligibility',
            'Type' => "enum('yes','no')"
        ),
        'probucol_eligibility_status' => array(
            'Description' => 'Probucol Eligibility Status',
            'Type' => "enum('active','stop_medication_active','withdrawn','excluded','death','completed','stop_medication_completed')"
        ),
        'probucol_eligibility_reason_specify' => array(
            'Description' => 'Probucol Eligibility Reason',
            'Type' => "text"
        ),
        'probucol_eligibility_reason_specify_status' => array(
            'Description' => 'Probucol Eligibility Reason status',
            'Type' => "enum('not_answered')"
        ),
        'probucol_excluded_reason_specify' => array(
            'Description' => 'Probucol Excluded Reason',
            'Type' => "text"
        ),
        'probucol_excluded_reason_specify_status' => array(
            'Description' => 'Probucol Excluded Reason status',
            'Type' => "enum('not_answered')"
        ),
        'probucol_withdrawal_reasons' => array(
            'Description' => 'Probucol Withdrawal Reason',
            'Type' => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')"
        ),
        'probucol_withdrawal_reasons_other_specify' => array(
            'Description' => 'Probucol Withdrawal Reason Other',
            'Type' => "text"
        ),
        'probucol_withdrawal_reasons_other_specify_status' => array(
            'Description' => 'Probucol Withdrawal Reason Other status',
            'Type' => "enum('not_answered')"
        ),
        'naproxen_ITT' => array(
            'Description' => 'Naproxen ITT',
            'Type' => "enum('yes','no')"
        ),
        'naproxen_mITT' => array(
            'Description' => 'Naproxen mITT',
            'Type' => "enum('yes','no')"
        ),
        'MCI_converter' => array(
            'Description' => 'MCI converter',
            'Type' => "enum('yes','no')"
        ),
        'MCI_converter_confirmed_onset' => array(
            'Description' => 'MCI converter confirmed onset',
            'Type' => "varchar(255)"
        ),
        'naproxen_treatment_assignment' => array(
            'Description' => 'Naproxen Treatment Assignment',
            'Type' => "enum('placebo','naproxen sodium 220 mg b.i.d.')"
        ),
	'ApoE' => array(
	    'Description' => 'ApoE genotype (E4 = risk)',
	    'Type' => "varchar(255)"
	),
	'apoE_allele_no' => array(
	    'Description' => 'Number of ApoE 4 allele (0, 1 or 2)',
	    'Type' => "int(10)"
	),
	'E4_allele_Bin' => array(
	    'Description' => 'Presence of ApoE 4 allele (0 = no ApoE 4 allele, 1 = at least one ApoE 4 allele)',
	    'Type' => "int(1)"
	),
	'Technicien_ApoE' => array(
	    'Description' => 'Technicien who ran ApoE analyses',
	    'Type' => "varchar(255)"
	),
	'Method_ApoE' => array(
	    'Description' => 'Method used to extract ApoE genotypes',
	    'Type' => "varchar(255)"
	),
	'Reference_ApoE' => array(
	    'Description' => 'Reference used to extract ApoE genotypes',
	    'Type' => "date"
	),
	'BchE_K_variant' => array(
	    'Description' => 'BchE K variant (A= K variant = risk)',
	    'Type' => "varchar(255)"
	),
	'K_variant_copie_no' => array(
	    'Description' => 'Number of BchE K allele (0, 1 or 2)',
	    'Type' => "int(10)"
	),
	'K_variant_bin' => array(
	    'Description' => 'Presence of BchE K allele (0 = no K variant; 1 = at least one K variant)',
	    'Type' => "int(1)"
	),
	'Technicien_BchE' => array(
	    'Description' => 'Technicien who ran BchE analyses',
	    'Type' => "varchar(255)"
	),
	'Method_BchE' => array(
	    'Description' => 'Method used to extract BchE variants',
	    'Type' => "varchar(255)"
	),
	'Reference_BchE' => array(
	    'Description' => 'Reference used to extract BchE variants',
	    'Type' => "date"
	),
	'BDNF' => array(
	    'Description' => 'BDNF genotype (A = mutation = risk)',
	    'Type' => "varchar(255)"
	),
	'BDNF_allele_no' => array(
	    'Description' => 'Number of BDNF A allele (0, 1 or 2)',
	    'Type' => "int(10)"
	),
	'BDNF_copie_bin' => array(
	    'Description' => 'Presence of BDNF A allele (0 = no BDNF A allele, 1 = at least one BDNF A allele)',
	    'Type' => "int(1)"
	),
	'Technicien_BDNF' => array(
	    'Description' => 'Technicien who ran BDNF analyses',
	    'Type' => "varchar(255)"
	),
	'Method_BDNF' => array(
	    'Description' => 'Method used to extract BDNF genotypes',
	    'Type' => "varchar(255)"
	),
	'Reference_BDNF' => array(
	    'Description' => 'Reference used to extract BDNF genotypes',
	    'Type' => "date"
	),
	'HMGR_Intron_M' => array(
	    'Description' => 'HMGR intron M genotype (TT is protective, just one T is not protective)',
	    'Type' => "varchar(255)"
	),
	'Intron_M_allele_no' => array(
	    'Description' => 'Number of HMGR Intron M T allele (0, 1 or 2) (2 = protection)',
	    'Type' => "int(10)"
	),
        'Intron_M_protective' => array(
            'Description' => 'Protective variant if ApoE4 and HMGR = TT (0 = less than 2 T allele, 1  = two T allele)  ',
            'Type' => "varchar(255)"
        ),
	'Technicien_M' => array(
	    'Description' => 'Technicien who ran HMGR analyses',
	    'Type' => "varchar(255)"
	),
	'Method_M' => array(
	    'Description' => 'Method used to extract HMGR genotypes',
	    'Type' => "varchar(255)"
	),
	'Reference_M' => array(
	    'Description' => 'Reference used to extract HMGR genotypes',
	    'Type' => "date"
	),
        'TLR4_rs_4986790' => array(
            'Description' => 'TLR4 genotype (G = risk, dose effect of this mutation so should use 0, 1 and 2 for analyses))',
            'Type' => "varchar(255)"
        ),
        'TLR4_allele_no' => array(
            'Description' => 'Number of TLR4 G allele (0, 1 or 2)',
            'Type' => "varchar(255)"
        ),
        'Technicien_TLR4' => array(
            'Description' => 'Technicien who ran TLR4 analyses',
            'Type' => "varchar(255)"
        ),
        'Method_TLR4' => array(
            'Description' => 'Method used to extract TLR4 genotypes',
            'Type' => "varchar(255)"
        ),
        'Reference_TLR4' => array(
            'Description' => 'Reference used to extract TLR4 genotypes',
            'Type' => "date"
        ),
        'PPP2r1A_rs_10406151' => array(
            'Description' => 'PPP2r1Ars10406151 genotype (T = mutation = risk)',
            'Type' => "varchar(255)"
        ),
        'ppp2r1A_allele_no' => array(
            'Description' => 'Number of PPP2r1A T  allele (0, 1 or 2)',
            'Type' => "varchar(255)"
        ),
        'ppp2r1A_copie_no' => array(
            'Description' => 'Presence of two copies of PPP2r1A T alleles (0 =no T allele, 1 = at least one T allele)',
            'Type' => "varchar(255)"
        ),
        'Technicien_ppp2r1a' => array(
            'Description' => 'Technicien who ran PPP2r1A analyses',
            'Type' => "varchar(255)"
        ),
        'Method_ppp2r1a' => array(
            'Description' => 'Method used to extract PPP2r1A genotypes',
            'Type' => "varchar(255)"
        ),
        'Reference_ppp2r1a' => array(
            'Description' => 'Reference used to extract PPP2r1A genotypes',
            'Type' => "date"
        ),
        'CDK5RAP2_rs10984186' => array(
            'Description' => 'Regulator of Tau phosphorylation',
            'Type' => "varchar(255)"
        ),
        'CDK5RAP2_rs10984186_allele_no' => array(
            'Description' => 'Number of T  allele (0, 1 or 2) ',
            'Type' => "varchar(255)"
        ),
        'CDK5RAP2_rs10984186_allele_bin' => array(
            'Description' => 'Presence of T allele (0 = no T allele, 1 = at least one T allele) ',
            'Type' => "varchar(255)"
        ),
        'Technicien_CDK5RAP2' => array(
            'Description' => 'Technicien who ran CDK5RAP2 analyses',
            'Type' => "varchar(255)"
        ),
        'Method_CDK5RAP2' => array(
            'Description' => 'Method used to extract CDK5RAP2 genotypes',
            'Type' => "varchar(255)"
        ),
        'Reference_CDK5RAP2' => array(
            'Description' => 'Reference used to extract CDK5RAP2 genotypes',
            'Type' => "date"
        ),
        'comments' => array(
            'Description' => 'Comments (wether DNA has been destroyed for example)',
            'Type' => "varchar(255)"
        ),
        'dna_collected_eligibility' => array(
            'Description' => 'DNA collected at Eligibility',
            'Type' => "enum('yes','no','not_answered')"
        ),
        'dna_request_destroy' => array(
            'Description' => 'DNA destruction request',
            'Type' => "enum('yes','no','not_answered')"
        ),
        'dna_destroy_date' => array(
            'Description' => 'DNA destruction date',
            'Type' => "date"
        ),
        'dna_destroy_date_status' => array(
            'Description' => 'DNA destruction date status',
            'Type' => "enum('not_answered')"
        ),
        'scan_done' => array(
            'Description' => 'Scan done',
            'Type' => "enum('N', 'Y')"
        ),
    );

    var $Config = array(
        'Meta' => array(
            'DocType' => 'ServerConfig'
        ),
        'Config' => array(
            'GroupString'  => 'How to arrange data: ',
            'GroupOptions' => 
                array('Cross-sectional', 'Longitudinal')
        )
    );

    function __construct() {
        $this->SQLDB = Database::singleton();
        $this->CouchDB = CouchDB::singleton();
    }

    function _getSubproject($id) {
        $config = NDB_Config::singleton();
        $subprojs = $config->getSubprojectSettings($id);
        if($subprojs['id'] == $id) {
            return $subprojs['title'];
        }
    }

    function _getProject($id) {
        $config = NDB_Config::singleton();
        $projs = $config->getProjectSettings($id);
        if($projs['id'] == $id) {
            return $projs['Name'];
        }
    }

    function _generateQuery() {
        $config = NDB_Config::singleton();
        $fieldsInQuery = "SELECT withdrawal_reasons, naproxen_eligibility, naproxen_eligibility_status, naproxen_eligibility_reason_specify, naproxen_eligibility_reason_specify_status, naproxen_excluded_reason_specify, naproxen_excluded_reason_specify_status, naproxen_withdrawal_reasons, naproxen_withdrawal_reasons_other_specify, naproxen_withdrawal_reasons_other_specify_status, probucol_eligibility, probucol_eligibility_status, probucol_eligibility_reason_specify, probucol_eligibility_reason_specify_status, probucol_excluded_reason_specify, probucol_excluded_reason_specify_status, probucol_withdrawal_reasons, probucol_withdrawal_reasons_other_specify, probucol_withdrawal_reasons_other_specify_status, naproxen_ITT, naproxen_mITT, MCI_converter, MCI_converter_confirmed_onset, naproxen_treatment_assignment, ps.entry_staff, ps.data_changed_date, ps.data_entry_date, pso.description,ps.reason_specify, ps.reason_specify_status, ps.withdrawal_reasons_other_specify, ps.withdrawal_reasons_other_specify_status, scan_done, ApoE, apoE_allele_no, E4_allele_Bin, Technicien_ApoE, Method_ApoE, Reference_ApoE, BchE_K_variant, K_variant_copie_no, K_variant_bin, Technicien_BchE, Method_BchE, Reference_BchE, BDNF, BDNF_allele_no, BDNF_copie_bin, Technicien_BDNF, Method_BDNF, Reference_BDNF, HMGR_Intron_M, Intron_M_allele_no, Technicien_M, Method_M, Reference_M, TLR4_rs_4986790, TLR4_allele_no, Technicien_TLR4, Method_TLR4, Reference_TLR4, PPP2r1A_rs_10406151, ppp2r1A_allele_no, ppp2r1A_copie_no, Technicien_ppp2r1a, Method_ppp2r1a, Reference_ppp2r1a, CDK5RAP2_rs10984186, CDK5RAP2_rs10984186_allele_no, CDK5RAP2_rs10984186_allele_bin, Technicien_CDK5RAP2, Method_CDK5RAP2, Reference_CDK5RAP2, g.comments, dna_collected_eligibility, dna_request_destroy, dna_destroy_date, dna_destroy_date_status, c.CandID, c.PSCID, s.Visit_label, s.SubprojectID, p.Alias as Site, c.Gender, c.Mother_tongue, c.DoB, s.Current_stage, s.Visit, CASE WHEN s.Visit='Failure' THEN 'Failure' WHEN s.Screening='Failure' THEN 'Failure' WHEN s.Visit='Withdrawal' THEN 'Withdrawal' WHEN s.Screening='Withdrawal' THEN 'Withdrawal' ELSE 'Neither' END as Failure, c.ProjectID, pso.Description as Status";
        $tablesToJoin = " FROM session s JOIN candidate c USING (CandID) LEFT JOIN psc p ON (p.CenterID=s.CenterID) LEFT JOIN parameter_type pt_plan ON (pt_plan.Name='candidate_plan') LEFT JOIN parameter_candidate AS pc_plan ON (pc_plan.CandID=c.CandID AND pt_plan.ParameterTypeID=pc_plan.ParameterTypeID) LEFT JOIN participant_status ps ON c.CandID=ps.CandID LEFT JOIN participant_status_options as pso ON ps.participant_status=pso.ID LEFT JOIN genetics as g ON g.PSCID=c.PSCID";
        // If proband fields are being used, add proband information into the query
        if ($config->getSetting("useProband") === "true") {
            $probandFields = ", c.ProbandGender as Gender_proband, ROUND(DATEDIFF(c.DoB, c.ProbandDoB) / (365/12)) AS Age_difference";
            $fieldsInQuery .= $probandFields;
        }
        // If expected date of confinement is being used, add EDC information into the query
        if ($config->getSetting("useEDC") === "true") {
            $EDCFields = ", c.EDC as EDC";
            $fieldsInQuery .= $EDCFields;
        }
        $concatQuery = $fieldsInQuery . $tablesToJoin . " WHERE s.Active='Y' AND c.Active='Y' AND c.Entity_type != 'Scanner'";
        return $concatQuery;
    }

    function _updateDataDict() {
        $config = NDB_Config::singleton();
        // If proband fields are being used, update the data dictionary
        if ($config->getSetting("useProband") === "true") {
            $this->Dictionary["Gender_proband"] = array(
                'Description' => 'Proband\'s gender',
                'Type' => "enum('Male','Female')"
            );
            $this->Dictionary["Age_difference"] = array(
                'Description' => 'Age difference between the candidate and the proband',
                'Type' => "int"
            );
        }
        // If expected date of confinement is being used, update the data dictionary
        if ($config->getSetting("useEDC") === "true") {
            $this->Dictionary["EDC"] = array(
                'Description' => 'Expected Date of Confinement (Due Date)',
                'Type' => "varchar(255)"
            );
        }
        if ($config->getSetting("useProjects") === "true") {
            $projects = Utility::getProjectList();
            $projectsEnum = "enum('";
            $projectsEnum .= implode("', '", $projects);
            $projectsEnum .= "')";
            $this->Dictionary["Project"] = array(
                'Description' => 'Project for which the candidate belongs',
                'Type' => $projectsEnum
            );
        }
        /*
        // Add any candidate parameter fields to the data dictionary
        $parameterCandidateFields = $this->SQLDB->pselect("SELECT * from parameter_type WHERE SourceFrom='parameter_candidate' AND Queryable=1",
            array());
        foreach($parameterCandidateFields as $field) {
            if(isset($field['Name'])) {
                $fname = $field['Name'];
                $Dict[$fname] = array();
                $Dict[$fname]['Description'] = $field['Description'];
                $Dict[$fname]['Type'] = $field['Type'];
            }
        }
        */
    }

    function run() {
        $config = $this->CouchDB->replaceDoc('Config:BaseConfig', $this->Config);
        print "Updating Config:BaseConfig: $config";

        // Run query
        $demographics = $this->SQLDB->pselect($this->_generateQuery(), array());

        $this->CouchDB->beginBulkTransaction();
        $config_setting = NDB_Config::singleton();
        foreach($demographics as $demographics) {
            $id = 'Demographics_Session_' . $demographics['PSCID'] . '_' . $demographics['Visit_label'];
            $demographics['Cohort'] = $this->_getSubproject($demographics['SubprojectID']);
            unset($demographics['SubprojectID']);
            if(isset($demographics['ProjectID'])) {
                $demographics['Project'] = $this->_getProject($demographics['ProjectID']);
                unset($demographics['ProjectID']);
            }

            // participant status history
            $exists = $this->SQLDB->pselectOne("SELECT ID FROM participant_status_history WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
            if (!empty($exists)) {
                $entries = 1;
                $status_history_fields = $this->SQLDB->pselect("SELECT * FROM participant_status_history WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
                foreach ($status_history_fields as $row) {
                    $this->Dictionary["Status_history_CandID_" . $entries] = array(
                           'Description' => "Status History CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["entry_staff_history_" . $entries] = array(
                           'Description' => "Participant status entry staff history " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["data_changed_date_history_" . $entries] = array(
                           'Description' => "Participant status data changed date history " . $entries,
                           'Type'        => "date",
                    );
                    $this->Dictionary["data_entry_date_history_" . $entries] = array(
                           'Description' => "Participant status data entry date history " . $entries,
                           'Type'        => "date",
                    );
                    $this->Dictionary["participant_status_history_" . $entries] = array(
                           'Description' => "Participant status history " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["reason_specify_history_" . $entries] = array(
                           'Description' => "Participant status reason history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["reason_specify_status_history_" . $entries] = array(
                           'Description' => "Participant status reason status history " . $entries,
                           'Type'        => "enum('dnk','not_applicable','refusal','not_answered')",
                    );
                    $this->Dictionary["withdrawal_reasons_history_" . $entries] = array(
                           'Description' => "Participant status withdrawal reason history " . $entries,
                           'Type'        => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')",
                    );
                    $this->Dictionary["withdrawal_reasons_other_specify_history_" . $entries] = array(
                           'Description' => "Other reason for withdrawal history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["withdrawal_reasons_other_specify_status_history_" . $entries] = array(
                           'Description' => "Other reason for withdrawal status history " . $entries,
                           'Type'        => "enum('dnk','not_applicable','refusal','not_answered'",
                    );
                    $this->Dictionary["naproxen_eligibility_history_" . $entries] = array(
                           'Description' => "Naproxen Eligibility history " . $entries,
                           'Type'        => "enum('yes','no')",
                    );
                    $this->Dictionary["naproxen_eligibility_reason_specify_history_" . $entries] = array(
                           'Description' => "Naproxen Eligibility Reason history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["naproxen_eligibility_reason_specify_status_history_" . $entries] = array(
                           'Description' => "Naproxen Eligibility Reason status history " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["naproxen_eligibility_status_history_" . $entries] = array(
                           'Description' => "Naproxen Eligibility Status history " . $entries,
                           'Type'        => "enum('active','stop_medication_active','withdrawn','excluded','death','completed','stop_medication_completed')",
                    );
                    $this->Dictionary["naproxen_excluded_reason_specify_history_" . $entries] = array(
                           'Description' => "Naproxen Excluded Reason history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["naproxen_excluded_reason_specify_status_history_" . $entries] = array(
                           'Description' => "Naproxen Excluded Reason status history " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["naproxen_withdrawal_reasons_history_" . $entries] = array(
                           'Description' => "Naproxen Withdrawal Reason history " . $entries,
                           'Type'        => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')",
                    );
                    $this->Dictionary["naproxen_withdrawal_reasons_other_specify_history_" . $entries] = array(
                           'Description' => "Naproxen Withdrawal Reason Other history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["naproxen_withdrawal_reasons_other_specify_status_history_" . $entries] = array(
                           'Description' => "Naproxen Withdrawal Reason Other status history " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["probucol_eligibility_history_" . $entries] = array(
                           'Description' => "Probucol Eligibility history " . $entries,
                           'Type'        => "enum('yes','no')",
                    );
                    $this->Dictionary["probucol_eligibility_reason_specify_history_" . $entries] = array(
                           'Description' => "Probucol Eligibility Reason history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["probucol_eligibility_reason_specify_status_history_" . $entries] = array(
                           'Description' => "Probucol Eligibility Reason status history " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["probucol_eligibility_status_history_" . $entries] = array(
                           'Description' => "Probucol Eligibility Status history " . $entries,
                           'Type'        => "enum('active','stop_medication_active','withdrawn','excluded','death','completed','stop_medication_completed')",
                    );
                    $this->Dictionary["probucol_excluded_reason_specify_history_" . $entries] = array(
                           'Description' => "Probucol Excluded Reason history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["probucol_excluded_reason_specify_status_history_" . $entries] = array(
                           'Description' => "Probucol Excluded Reason status history " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["probucol_withdrawal_reasons_history_" . $entries] = array(
                           'Description' => "Probucol Withdrawal Reason history " . $entries,
                           'Type'        => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')",
                    );
                    $this->Dictionary["probucol_withdrawal_reasons_other_specify_history_" . $entries] = array(
                           'Description' => "Probucol Withdrawal Reason Other history " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["probucol_withdrawal_reasons_other_specify_status_history_" . $entries] = array(
                           'Description' => "Probucol Withdrawal Reason Other status history " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["naproxen_ITT_history_" . $entries] = array(
                           'Description' => "Naproxen ITT history " . $entries,
                           'Type'        => "enum('yes','no')",
                    );
                    $this->Dictionary["naproxen_mITT_history_" . $entries] = array(
                           'Description' => "Naproxen mITT history " . $entries,
                           'Type'        => "enum('yes','no')",
                    );
                    $this->Dictionary["MCI_converter_history_" . $entries] = array(
                           'Description' => "MCI converter history " . $entries,
                           'Type'        => "enum('yes','no')",
                    );
                    $this->Dictionary["MCI_converter_confirmed_onset_history_" . $entries] = array(
                           'Description' => "MCI converter confirmed onset history " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["naproxen_treatment_assignment_history_" . $entries] = array(
                           'Description' => "Naproxen Treatment Assignment history " . $entries,
                           'Type'        => "enum('placebo','naproxen sodium 220 mg b.i.d.')",
                    );
                    $demographics["Status_history_CandID_" . $entries] = $row['CandID'];
                    $demographics["entry_staff_history_" . $entries] = $row['entry_staff'];
                    $demographics["data_changed_date_history_" . $entries] = $row['data_changed_date'];
                    $demographics["data_entry_date_history_" . $entries] = $row['data_entry_date'];
                    $demographics["participant_status_history_" . $entries] = $row['participant_status'];
                    $demographics["reason_specify_history_" . $entries] = $row['reason_specify'];
                    $demographics["reason_specify_status_history_" . $entries] = $row['reason_specify_status'];
                    $demographics["withdrawal_reasons_history_" . $entries] = $row['withdrawal_reasons'];
                    $demographics["withdrawal_reasons_other_specify_history_" . $entries] = $row['withdrawal_reasons_other_specify'];
                    $demographics["withdrawal_reasons_other_specify_status_history_" . $entries] = $row['withdrawal_reasons_other_specify_status'];
                    $demographics["naproxen_eligibility_history_" . $entries] = $row['naproxen_eligibility'];
                    $demographics["naproxen_eligibility_reason_specify_history_" . $entries] = $row['naproxen_eligibility_reason_specify'];
                    $demographics["naproxen_eligibility_reason_specify_status_history_" . $entries] = $row['naproxen_eligibility_reason_specify_status'];
                    $demographics["naproxen_eligibility_status_history_" . $entries] = $row['naproxen_eligibility_status'];
                    $demographics["naproxen_excluded_reason_specify_history_" . $entries] = $row['naproxen_excluded_reason_specify'];
                    $demographics["naproxen_excluded_reason_specify_status_history_" . $entries] = $row['naproxen_excluded_reason_specify_status'];
                    $demographics["naproxen_withdrawal_reasons_history_" . $entries] = $row['naproxen_withdrawal_reasons'];
                    $demographics["naproxen_withdrawal_reasons_other_specify_history_" . $entries] = $row['naproxen_withdrawal_reasons_other_specify'];
                    $demographics["naproxen_withdrawal_reasons_other_specify_status_history_" . $entries] = $row['naproxen_withdrawal_reasons_other_specify_status'];
                    $demographics["probucol_eligibility_history_" . $entries] = $row['probucol_eligibility'];
                    $demographics["probucol_eligibility_reason_specify_history_" . $entries] = $row['probucol_eligibility_reason_specify'];
                    $demographics["probucol_eligibility_reason_specify_status_history_" . $entries] = $row['probucol_eligibility_reason_specify_status'];
                    $demographics["probucol_eligibility_status_history_" . $entries] = $row['probucol_eligibility_status'];
                    $demographics["probucol_excluded_reason_specify_history_" . $entries] = $row['probucol_excluded_reason_specify'];
                    $demographics["probucol_excluded_reason_specify_status_history_" . $entries] = $row['probucol_excluded_reason_specify_status'];
                    $demographics["probucol_withdrawal_reasons_history_" . $entries] = $row['probucol_withdrawal_reasons'];
                    $demographics["probucol_withdrawal_reasons_other_specify_history_" . $entries] = $row['probucol_withdrawal_reasons_other_specify'];
                    $demographics["probucol_withdrawal_reasons_other_specify_status_history_" . $entries] = $row['probucol_withdrawal_reasons_other_specify_status'];
                    $demographics["naproxen_ITT_history_" . $entries] = $row['naproxen_ITT'];
                    $demographics["naproxen_mITT_history_" . $entries] = $row['naproxen_mITT'];
                    $demographics["MCI_converter_history_" . $entries] = $row['MCI_converter'];
                    $demographics["MCI_converter_confirmed_onset_history_" . $entries] = $row['MCI_converter_confirmed_onset'];
                    $demographics["naproxen_treatment_assignment_history_" . $entries] = $row['naproxen_treatment_assignment'];
                    $entries++;
                }
            }

            // family history
            $exists = $this->SQLDB->pselectOne("SELECT ID FROM family_history_ad_other WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
            if (!empty($exists)) {
                $entries = 1;
                $family_history_ad_other_fields = $this->SQLDB->pselect("SELECT * FROM family_history_ad_other WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
                foreach ($family_history_ad_other_fields as $row) {
                    $this->Dictionary["ADOther_CandID_" . $entries] = array(
                           'Description' => "Family History AD Other CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["ADOther_family_member_" . $entries] = array(
                           'Description' => "Family History AD Other Family Member " . $entries,
                           'Type'        => "enum('aunt','uncle','niece','nephew','half-sibling','first-cousin','grandfather','grandmother','other')",
                    );
                    $this->Dictionary["ADOther_parental_side_" . $entries] = array(
                           'Description' => "Family History AD Other Parental Side " . $entries,
                           'Type'        => "enum('maternal','paternal','not_applicable','not_answered')",
                    );
                    $this->Dictionary["ADOther_ad_dementia_age_" . $entries] = array(
                           'Description' => "Family History AD Other AD Dementia Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["ADOther_living_age_" . $entries] = array(
                           'Description' => "Family History AD Other Living Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["ADOther_death_age_" . $entries] = array(
                           'Description' => "Family History AD Other Death Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["ADOther_death_cause_" . $entries] = array(
                           'Description' => "Family History AD Other Death Cause " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["ADOther_death_cause_status_" . $entries] = array(
                           'Description' => "Family History AD Other Death Cause Status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $demographics["ADOther_CandID_" . $entries] = $row['CandID'];
                    $demographics["ADOther_family_member_" . $entries] = $row['family_member'];
                    $demographics["ADOther_parental_side_" . $entries] = $row['parental_side'];
                    $demographics["ADOther_ad_dementia_age_" . $entries] = $row['ad_dementia_age'];
                    $demographics["ADOther_living_age_" . $entries] = $row['living_age'];
                    $demographics["ADOther_death_age_" . $entries] = $row['death_age'];
                    $demographics["ADOther_death_cause_" . $entries] = $row['death_cause'];
                    $demographics["ADOther_death_cause_status_" . $entries] = $row['death_cause_status'];
                    $entries++;
                }
            }

            $exists = $this->SQLDB->pselectOne("SELECT ID FROM family_history_first_degree WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
            if (!empty($exists)) {
                $entries = 1;
                $family_history_first_degree_fields = $this->SQLDB->pselect("SELECT * FROM family_history_first_degree WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
                foreach ($family_history_first_degree_fields as $row) {
                    $this->Dictionary["FirstDegree_CandID_" . $entries] = array(
                           'Description' => "Family History First Degree CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["FirstDegree_family_member_" . $entries] = array(
                           'Description' => "Family History First Degree Family Member " . $entries,
                           'Type'        => "enum('mother','father','brother','sister','son','daughter')",
                    );
                    $this->Dictionary["FirstDegree_living_age_" . $entries] = array(
                           'Description' => "Family History First Degree Living Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["FirstDegree_death_age_" . $entries] = array(
                           'Description' => "Family History First Degree Death Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["FirstDegree_death_cause_" . $entries] = array(
                           'Description' => "Family History First Degree Death Cause " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["FirstDegree_death_cause_status_" . $entries] = array(
                           'Description' => "Family History First Degree Death Cause Status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["FirstDegree_ad_dementia_" . $entries] = array(
                           'Description' => "Family History First Degree AD Dementia " . $entries,
                           'Type'        => "enum('Y','N','not_answered')",
                    );
                    $this->Dictionary["FirstDegree_ad_dementia_age_" . $entries] = array(
                           'Description' => "Family History First Degree AD Dementia Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["FirstDegree_diagnosis_history_" . $entries] = array(
                           'Description' => "Family History First Degree Diagnosis History " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["FirstDegree_diagnosis_history_status_" . $entries] = array(
                           'Description' => "Family History First Degree Diagnosis History Status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $demographics["FirstDegree_CandID_" . $entries] = $row['CandID'];
                    $demographics["FirstDegree_family_member_" . $entries] = $row['family_member'];
                    $demographics["FirstDegree_living_age_" . $entries] = $row['living_age'];
                    $demographics["FirstDegree_death_age_" . $entries] = $row['death_age'];
                    $demographics["FirstDegree_death_cause_" . $entries] = $row['death_cause'];
                    $demographics["FirstDegree_death_cause_status_" . $entries] = $row['death_cause_status'];
                    $demographics["FirstDegree_ad_dementia_" . $entries] = $row['ad_dementia'];
                    $demographics["FirstDegree_ad_dementia_age_" . $entries] = $row['ad_dementia_age'];
                    $demographics["FirstDegree_diagnosis_history_" . $entries] = $row['diagnosis_history'];
                    $demographics["FirstDegree_diagnosis_history_status_" . $entries] = $row['diagnosis_history_status'];
                    $entries++;
                }
            }

            $exists = $this->SQLDB->pselectOne("SELECT ID FROM family_history_memory_problem_other WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
            if (!empty($exists)) {
                $entries = 1;
                $family_history_memory_problem_other_fields = $this->SQLDB->pselect("SELECT * FROM family_history_memory_problem_other WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
                foreach ($family_history_memory_problem_other_fields as $row) {
                    $this->Dictionary["MemoryProblemOther_CandID_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["MemoryProblemOther_family_member_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Family Member " . $entries,
                           'Type'        => "enum('grandmother','grandfather','aunt','uncle','niece','nephew','half-sibling','first-cousin','other')",
                    );
                    $this->Dictionary["MemoryProblemOther_parental_side_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Parental Side " . $entries,
                           'Type'        => "enum('maternal','paternal','not_applicable','not_answered')",
                    );
                    $this->Dictionary["MemoryProblemOther_other_memory_problems_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Other Memory Problems " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["MemoryProblemOther_other_memory_problems_status_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Other Memory Problems Status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["MemoryProblemOther_living_age_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Living Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["MemoryProblemOther_death_age_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Death Age " . $entries,
                           'Type'        => "enum('0','1','2','3','4','5','6','7','8','9','10','11','12','13','14','15','16','17','18','19','20','21','22','23','24','25','26','27','28','29','30','31','32','33','34','35','36','37','38','39','40','41','42','43','44','45','46','47','48','49','50','51','52','53','54','55','56','57','58','59','60','61','62','63','64','65','66','67','68','69','70','71','72','73','74','75','76','77','78','79','80','81','82','83','84','85','86','87','88','89','90','91','92','93','94','95','96','97','98','99','100','101','102','103','104','105','106','107','108','109','110','111','112','113','114','115','116','117','118','119','120','not_applicable','not_answered')",
                    );
                    $this->Dictionary["MemoryProblemOther_death_cause_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Death Cause " . $entries,
                           'Type'        => "text",
                    );
                    $this->Dictionary["MemoryProblemOther_death_cause_status_" . $entries] = array(
                           'Description' => "Family History Memory Problem Other Death Cause Status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $demographics["MemoryProblemOther_CandID_" . $entries] = $row['CandID'];
                    $demographics["MemoryProblemOther_family_member_" . $entries] = $row['family_member'];
                    $demographics["MemoryProblemOther_parental_side_" . $entries] = $row['parental_side'];
                    $demographics["MemoryProblemOther_other_memory_problems_" . $entries] = $row['other_memory_problems'];
                    $demographics["MemoryProblemOther_other_memory_problems_status_" . $entries] = $row['other_memory_problems_status'];
                    $demographics["MemoryProblemOther_living_age_" . $entries] = $row['living_age'];
                    $demographics["MemoryProblemOther_death_age_" . $entries] = $row['death_age'];
                    $demographics["MemoryProblemOther_death_cause_" . $entries] = $row['death_cause'];
                    $demographics["MemoryProblemOther_death_cause_status_" . $entries] = $row['death_cause_status'];
                    $entries++;
                }
            }

            // family_information
            $exists = $this->SQLDB->pselectOne("SELECT ID FROM family_information WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
            if (!empty($exists)) {
                $entries = 1;
                $family_information_fields = $this->SQLDB->pselect("SELECT CandID, related_participant_PSCID, related_participant_CandID, related_participant_status_degree, related_participant_status, related_participant_status_specify FROM family_information WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
                foreach ($family_information_fields as $row) {
                    $this->Dictionary["participant_CandID_" . $entries] = array(
                           'Description' => "Participant's CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["related_participant_PSCID_" . $entries] = array(
                           'Description' => "Related participant's PSCID " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["related_participant_CandID_" . $entries] = array(
                           'Description' => "Related participant's CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["related_participant_status_degree_" . $entries] = array(
                           'Description' => "Related participant's status degree " . $entries,
                           'Type'        => "enum('1st degree relative','2nd degree relative','3rd degree relative','other')",
                    );
                    $this->Dictionary["related_participant_status_" . $entries] = array(
                           'Description' => "Related participant's status " . $entries,
                           'Type'        => "enum('sibling','parent','child','aunt/uncle','niece/nephew','half-sibling','first-cousin','other')",
                    );
                    $this->Dictionary["related_participant_status_specify_" . $entries] = array(
                           'Description' => "Related participant's status specify " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $demographics["participant_CandID_" . $entries] = $row['CandID'];
                    $demographics["related_participant_PSCID_" . $entries] = $row['related_participant_PSCID'];
                    $demographics["related_participant_CandID_" . $entries] = $row['related_participant_CandID'];
                    $demographics["related_participant_status_degree_" . $entries] = $row['related_participant_status_degree'];
                    $demographics["related_participant_status_" . $entries] = $row['related_participant_status'];
                    $demographics["related_participant_status_specify_" . $entries] = $row['related_participant_status_specify'];
                    $entries++;
                }
            }

            // drug compliance
            $exists = $this->SQLDB->pselectOne("SELECT ID FROM drug_compliance WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
            if (!empty($exists)) {
                $entries = 1;
                $drug_compliance_fields = $this->SQLDB->pselect("SELECT CandID, drug, visit_label, dosage, drug_issued_date, drug_issued_date_status, pills_issued, pills_issued_status, drug_returned_date, drug_returned_date_status, pills_returned, pills_returned_status, compliance_evaluation, compliance_evaluation_status, behavioral_compliance_evaluation, behavioral_compliance_evaluation_status, comments, comments_status FROM drug_compliance WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
                foreach ($drug_compliance_fields as $row) {
                    $this->Dictionary["drug_compliance_CandID_" . $entries] = array(
                           'Description' => "Drug Compliance CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["drug_compliance_drug_" . $entries] = array(
                           'Description' => "Drug Compliance drug " . $entries,
                           'Type'        => "enum('naproxen','probucol')",
                    );
                    $this->Dictionary["drug_compliance_visit_label_" . $entries] = array(
                           'Description' => "Drug Compliance visit label " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["drug_compliance_dosage_" . $entries] = array(
                           'Description' => "Drug Compliance dosage " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["drug_compliance_drug_issued_date_" . $entries] = array(
                           'Description' => "Drug Compliance drug issued date " . $entries,
                           'Type'        => "date",
                    );
                    $this->Dictionary["drug_compliance_drug_issued_date_status_" . $entries] = array(
                           'Description' => "Drug Compliance drug issued date status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["drug_compliance_pills_issued_" . $entries] = array(
                           'Description' => "Drug Compliance pills issued " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["drug_compliance_pills_issued_status_" . $entries] = array(
                           'Description' => "Drug Compliance pills issued status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["drug_compliance_drug_returned_date_" . $entries] = array(
                           'Description' => "Drug Compliance drug returned date " . $entries,
                           'Type'        => "date",
                    );
                    $this->Dictionary["drug_compliance_drug_returned_date_status_" . $entries] = array(
                           'Description' => "Drug Compliance drug returned date status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["drug_compliance_pills_returned_" . $entries] = array(
                           'Description' => "Drug Compliance pills returned " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["drug_compliance_pills_returned_status_" . $entries] = array(
                           'Description' => "Drug Compliance pills returned status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["drug_compliance_compliance_evaluation_" . $entries] = array(
                           'Description' => "Drug Compliance compliance evaluation " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["drug_compliance_compliance_evaluation_status_" . $entries] = array(
                           'Description' => "Drug Compliance compliance evaluation status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["drug_compliance_behavioral_compliance_evaluation_" . $entries] = array(
                           'Description' => "Drug Compliance behavioral compliance evaluation " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["drug_compliance_behavioral_compliance_evaluation_status_" . $entries] = array(
                           'Description' => "Drug Compliance behavioral compliance evaluation status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );
                    $this->Dictionary["drug_compliance_comments_" . $entries] = array(
                           'Description' => "Drug Compliance comments " . $entries,
                           'Type'        => "varchar(255)",
                    );
                    $this->Dictionary["drug_compliance_comments_status_" . $entries] = array(
                           'Description' => "Drug Compliance comments status " . $entries,
                           'Type'        => "enum('not_answered')",
                    );

                    $demographics["drug_compliance_CandID_" . $entries] = $row['CandID'];
                    $demographics["drug_compliance_drug_" . $entries] = $row['drug'];
                    $demographics["drug_compliance_dosage_" . $entries] = $row['dosage'];
                    $demographics["drug_compliance_visit_label_" . $entries] = $row['visit_label'];
                    $demographics["drug_compliance_drug_issued_date_" . $entries] = $row['drug_issued_date'];
                    $demographics["drug_compliance_drug_issued_date_status_" . $entries] = $row['drug_issued_date_status'];
                    $demographics["drug_compliance_pills_issued_" . $entries] = $row['pills_issued'];
                    $demographics["drug_compliance_pills_issued_status_" . $entries] = $row['pills_issued_status'];
                    $demographics["drug_compliance_drug_returned_date_" . $entries] = $row['drug_returned_date'];
                    $demographics["drug_compliance_drug_returned_date_status_" . $entries] = $row['drug_returned_date_status'];
                    $demographics["drug_compliance_pills_returned_" . $entries] = $row['pills_returned'];
                    $demographics["drug_compliance_pills_returned_status_" . $entries] = $row['pills_returned_status'];
                    $demographics["drug_compliance_compliance_evaluation_" . $entries] = $row['compliance_evaluation'];
                    $demographics["drug_compliance_compliance_evaluation_status_" . $entries] = $row['compliance_evaluation_status'];
                    $demographics["drug_compliance_behavioral_compliance_evaluation_" . $entries] = $row['behavioral_compliance_evaluation'];
                    $demographics["drug_compliance_behavioral_compliance_evaluation_status_" . $entries] = $row['behavioral_compliance_evaluation_status'];
                    $demographics["drug_compliance_comments_" . $entries] = $row['comments'];
                    $demographics["drug_compliance_comments_status_" . $entries] = $row['comments_status'];
                    $entries++;
                }
            }

            // treatment duration
            $exists = $this->SQLDB->pselectOne("SELECT ID FROM treatment_duration WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
            if (!empty($exists)) {
                $entries = 1;
                $treatment_duration_fields = $this->SQLDB->pselect("SELECT CandID, Trial, treatment_duration FROM treatment_duration WHERE CandID=:cid", array('cid'=>$demographics['CandID']));
                foreach ($treatment_duration_fields as $row) {
                    $this->Dictionary["treatment_duration_CandID_" . $entries] = array(
                           'Description' => "Treatment Duration CandID " . $entries,
                           'Type'        => "int(6)",
                    );
                    $this->Dictionary["treatment_duration_Trial_" . $entries] = array(
                           'Description' => "Treatment Duration Trial " . $entries,
                           'Type'        => "enum('Naproxen','Probucol_Phase_1_Trial_A')",
                    );
                    $this->Dictionary["treatment_duration_treatment_duration_" . $entries] = array(
                           'Description' => "Treatment Duration " . $entries,
                           'Type'        => "int(6)",
                    );

                    $demographics["treatment_duration_CandID_" . $entries] = $row['CandID'];
                    $demographics["treatment_duration_Trial_" . $entries] = $row['Trial'];
                    $demographics["treatment_duration_treatment_duration_" . $entries] = $row['treatment_duration'];
                    $entries++;
                }
            }


            if ($config_setting->getSetting("useFamilyID") === "true") {
                $familyID     = $this->SQLDB->pselectOne("SELECT FamilyID FROM family
                                                          WHERE CandID=:cid",
                                                          array('cid'=>$demographics['CandID']));
                if (!empty($familyID)) {
                   $this->Dictionary["FamilyID"] = array(
                                    'Description' => 'FamilyID of Candidate',
                                    'Type'        => "int(6)",
                                    );
                    $demographics['FamilyID'] = $familyID;
                    $familyFields = $this->SQLDB->pselect("SELECT candID as Family_ID,
                                    Relationship_type as Relationship_to_candidate
                                    FROM family
                                    WHERE FamilyID=:fid AND CandID<>:cid",
                                    array('fid'=>$familyID, 'cid'=>$demographics['CandID']));
                    $num_family = 1;
                    if (!empty($familyFields)) {
                        foreach($familyFields as $row) {
                            //adding each sibling id and relationship to the file
                            $this->Dictionary["Family_CandID".$num_family] = array(
                                    'Description' => 'CandID of Family Member '.$num_family,
                                    'Type'        => "varchar(255)",
                                    );
                            $this->Dictionary["Relationship_type_Family".$num_family] = array(
                                    'Description' => 'Relationship of candidate to Family Member '.$num_family,
                                    'Type'        => "enum('half_sibling','full_sibling','1st_cousin')",
                                    );
                            $demographics['Family_CandID'.$num_family]                      = $row['Family_ID'];
                            $demographics['Relationship_type_Family'.$num_family] = $row['Relationship_to_candidate'];
                            $num_family                                                += 1;
                        }
                    }
                }
            }

            $success = $this->CouchDB->replaceDoc($id, array('Meta' => array(
                'DocType' => 'demographics',
                'identifier' => array($demographics['PSCID'], $demographics['Visit_label'])
            ),
                'data' => $demographics
            ));
            print "$id: $success\n";
        }
        $this->_updateDataDict();
        $this->CouchDB->replaceDoc('DataDictionary:Demographics',
                array('Meta' => array('DataDict' => true),
                    'DataDictionary' => array('demographics' => $this->Dictionary)
                    )
                );

        print $this->CouchDB->commitBulkTransaction();

    }
}

// Don't run if we're doing the unit tests; the unit test will call run.
if(!class_exists('UnitTestCase')) {
    $Runner = new CouchDBDemographicsImporter();
    $Runner->run();
}
?>
