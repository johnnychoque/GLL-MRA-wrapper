
int MRAwrapper::processWSI_ACSF_registerFE(WSI_packet *packet)
{
    int ret_=0;

    DBG_IN("MRAwrapper::processWSI_ACSF_registerFE");    

    switch(packet->type()) {
    case MRRM_FE:
	DBG_INFO("Calling wrapper for MRRM registerFE");
	RegisterFE("MRRM");
	break;
    case GLL_FE:
	DBG_INFO("Calling wrapper for GLL registerFE");
	RegisterFE("GLL");
	break;
    default:
	DBG_ERR("Type unknown");
	ret_ = -1;
	break;
    }

    DBG_OUT("MRAwrapper::processWSI_ACSF_registerFE");
    return ret_;
}

int MRAwrapper::processWSI_ACSF_unRegisterFE(WSI_packet *packet)
{
    int ret_=0;

    DBG_IN("MRAwrapper::processWSI_ACSF_unregisterFE");    

    switch(packet->type()) {
    case MRRM_FE:
	DBG_INFO("Calling wrapper for MRRM unregisterFE");
	UnRegisterFE("MRRM");
	break;
    case GLL_FE:
	DBG_INFO("Calling wrapper for GLL unregisterFE");
	UnRegisterFE("GLL");
	break;
    default:
	DBG_ERR("Type unknown");
	ret_ = -1;
	break;
    }

    DBG_OUT("MRAwrapper::processWSI_ACSF_unregisterFE");
    return ret_;
}
