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
	'reason_specify' => array(
	    'Description' => 'Participant status reason',
	    'Type' => "text"
	),
        'withdrawal_reasons' => array(
            'Description' => 'Participant status withdrawal reason',
            'Type' => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')"
        ),
	'withdrawal_reasons_other_specify' => array(
	    'Description' => 'Other reason for withdrawal',
	    'Type' => "text"
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
        'naproxen_excluded_reason_specify' => array(
            'Description' => 'Naproxen Excluded Reason',
            'Type' => "text"
        ),
        'naproxen_withdrawal_reasons' => array(
            'Description' => 'Naproxen Withdrawal Reason',
            'Type' => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')"
        ),
        'naproxen_withdrawal_reasons_other_specify' => array(
            'Description' => 'Naproxen Withdrawal Reason Other',
            'Type' => "text"
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
        'probucol_excluded_reason_specify_status' => array(
            'Description' => 'Probucol Excluded Reason',
            'Type' => "text"
        ),
        'probucol_withdrawal_reasons' => array(
            'Description' => 'Probucol Withdrawal Reason',
            'Type' => "enum('1_voluntary_withdrawal','2_recommended_withdrawal','3_lost_follow_up','4_other')"
        ),
        'probucol_withdrawal_reasons_other_specify' => array(
            'Description' => 'Probucol Withdrawal Reason Other',
            'Type' => "text"
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
        'drug_compliance_drug' => array(
            'Description' => 'Drug Compliance drug',
            'Type' => "enum('naproxen','probucol')"
        ),
        'drug_compliance_visit_label' => array(
            'Description' => 'Drug Compliance visit label',
            'Type' => "varchar(255)"
        ),
        'drug_compliance_drug_issued_date' => array(
            'Description' => 'Drug Compliance drug issued date',
            'Type' => "date"
        ),
        'drug_compliance_drug_issued_date_status' => array(
            'Description' => 'Drug Compliance drug issued date status',
            'Type' => "enum('not_answered')"
        ),
        'drug_compliance_pills_issued' => array(
            'Description' => 'Drug Compliance pills issued',
            'Type' => "varchar(255)"
        ),
        'drug_compliance_pills_issued_status' => array(
            'Description' => 'Drug Compliance pills issued status',
            'Type' => "enum('not_answered')"
        ),
        'drug_compliance_drug_returned_date' => array(
            'Description' => 'Drug Compliance drug returned date',
            'Type' => "date"
        ),
        'drug_compliance_drug_returned_date_status' => array(
            'Description' => 'Drug Compliance drug returned date status',
            'Type' => "enum('not_answered')"
        ),
        'drug_compliance_pills_returned' => array(
            'Description' => 'Drug Compliance pills returned',
            'Type' => "varchar(255)"
        ),
        'drug_compliance_pills_returned_status' => array(
            'Description' => 'Drug Compliance pills returned status',
            'Type' => "enum('not_answered')"
        ),
        'drug_compliance_compliance_evaluation' => array(
            'Description' => 'Drug Compliance compliance evaluation',
            'Type' => "varchar(255)"
        ),
        'drug_compliance_compliance_evaluation_status' => array(
            'Description' => 'Drug Compliance compliance evaluation status',
            'Type' => "enum('not_answered')"
        ),
        'drug_compliance_behavioral_compliance_evaluation' => array(
            'Description' => 'Drug Compliance behavioral compliance evaluation',
            'Type' => "varchar(255)"
        ),
        'drug_compliance_behavioral_compliance_evaluation_status' => array(
            'Description' => 'Drug Compliance behavioral compliance evaluation status',
            'Type' => "enum('not_answered')"
        ),
        'drug_compliance_comments' => array(
            'Description' => 'Drug Compliance comments',
            'Type' => "varchar(255)"
        ),
        'drug_compliance_comments_status' => array(
            'Description' => 'Drug Compliance comments status',
            'Type' => "enum('not_answered')"
        ),
        'treatment_duration_trial' => array(
            'Description' => 'Treatment Duration Trial',
            'Type' => "enum('Naproxen','Probucol')"
        ),
        'treatment_duration' => array(
            'Description' => 'Treatment Duration',
            'Type' => "int(6)"
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
        $fieldsInQuery = "SELECT withdrawal_reasons, naproxen_eligibility, naproxen_eligibility_status, naproxen_eligibility_reason_specify, naproxen_excluded_reason_specify, naproxen_withdrawal_reasons, naproxen_withdrawal_reasons_other_specify, probucol_eligibility, probucol_eligibility_status, probucol_eligibility_reason_specify, probucol_excluded_reason_specify_status, probucol_withdrawal_reasons, probucol_withdrawal_reasons_other_specify, pso.description,ps.reason_specify, ps.withdrawal_reasons_other_specify, scan_done, ApoE, apoE_allele_no, E4_allele_Bin, Technicien_ApoE, Method_ApoE, Reference_ApoE, BchE_K_variant, K_variant_copie_no, K_variant_bin, Technicien_BchE, Method_BchE, Reference_BchE, BDNF, BDNF_allele_no, BDNF_copie_bin, Technicien_BDNF, Method_BDNF, Reference_BDNF, HMGR_Intron_M, Intron_M_allele_no, Technicien_M, Method_M, Reference_M, TLR4_rs_4986790, TLR4_allele_no, Technicien_TLR4, Method_TLR4, Reference_TLR4, PPP2r1A_rs_10406151, ppp2r1A_allele_no, ppp2r1A_copie_no, Technicien_ppp2r1a, Method_ppp2r1a, Reference_ppp2r1a, CDK5RAP2_rs10984186, CDK5RAP2_rs10984186_allele_no, CDK5RAP2_rs10984186_allele_bin, Technicien_CDK5RAP2, Method_CDK5RAP2, Reference_CDK5RAP2, g.comments, dna_collected_eligibility, dna_request_destroy, dna_destroy_date, dna_destroy_date_status, d.drug as 'drug_compliance_drug', d.visit_label as 'drug_compliance_visit_label', d.drug_issued_date as 'drug_compliance_drug_issued_date', d.drug_issued_date_status as 'drug_compliance_drug_issued_date_status', d.pills_issued as 'drug_compliance_pills_issued', d.pills_issued_status as 'drug_compliance_pills_issued_status', d.drug_returned_date as 'drug_compliance_drug_returned_date', d.drug_returned_date_status as 'drug_compliance_drug_returned_date_status', d.pills_returned as 'drug_compliance_pills_returned', d.pills_returned_status as 'drug_compliance_pills_returned_status', d.compliance_evaluation as 'drug_compliance_compliance_evaluation', d.compliance_evaluation_status as 'drug_compliance_compliance_evaluation_status', d.behavioral_compliance_evaluation as 'drug_compliance_behavioral_compliance_evaluation', d.behavioral_compliance_evaluation_status as 'drug_compliance_behavioral_compliance_evaluation_status', d.comments as 'drug_compliance_comments', d.comments_status as 'drug_compliance_comments_status', td.Trial as 'treatment_duration_trial', td.treatment_duration, c.CandID, c.PSCID, s.Visit_label, s.SubprojectID, p.Alias as Site, c.Gender, c.Mother_tongue, c.DoB, s.Current_stage, s.Visit, CASE WHEN s.Visit='Failure' THEN 'Failure' WHEN s.Screening='Failure' THEN 'Failure' WHEN s.Visit='Withdrawal' THEN 'Withdrawal' WHEN s.Screening='Withdrawal' THEN 'Withdrawal' ELSE 'Neither' END as Failure, c.ProjectID, pso.Description as Status";
        $tablesToJoin = " FROM session s JOIN candidate c USING (CandID) LEFT JOIN psc p ON (p.CenterID=s.CenterID) LEFT JOIN parameter_type pt_plan ON (pt_plan.Name='candidate_plan') LEFT JOIN parameter_candidate AS pc_plan ON (pc_plan.CandID=c.CandID AND pt_plan.ParameterTypeID=pc_plan.ParameterTypeID) LEFT JOIN participant_status ps ON c.CandID=ps.CandID LEFT JOIN participant_status_options as pso ON ps.participant_status=pso.ID LEFT JOIN genetics as g ON g.PSCID=c.PSCID LEFT JOIN drug_compliance as d ON c.CandID=d.CandID LEFT JOIN treatment_duration as td ON c.CandID=td.CandID";
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
        $concatQuery = $fieldsInQuery . $tablesToJoin . " WHERE s.Active='Y' AND c.Active='Y' AND c.PSCID <> 'scanner'";
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
