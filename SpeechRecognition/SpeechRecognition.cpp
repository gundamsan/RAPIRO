#include <stdio.h>

#include <julius/juliuslib.h>

#include "SerialPort.h"


// �V���A���ʐM��`
#define MODEMDEVICE "/dev/ttyAMA0"

// �V���A���|�[�g�I�u�W�F�N�g�錾�B
SerialPort rapiro(MODEMDEVICE);

// �F�����������i�����Ă�����ԁj�ɂȂ������̃R�[���o�b�N�֐���`�B
static void callback_speech_ready(Recog *recog, void *dummy)
{
	if (recog->jconf->input.speech_input == SP_MIC
		|| recog->jconf->input.speech_input  == SP_NETAUDIO)
	{
		fprintf(stderr, "<<< please speak >>>\n");
		//rapiro.writeBytes("#M0", 3);
	}
}

// �F�������J�n�������̃R�[���o�b�N�֐���`�B
static void callback_recognition_begin(Recog *recog, void *dummy)
{
	fprintf(stderr, "--- in recognition ---\n");
	//rapiro.writeBytes("#M8", 3);
}

// �F�����ʂ��o�͂���R�[���o�b�N�֐���`�B
static void callback_output_result(Recog *recog, void *dummy)
{
	WORD_INFO *winfo;
	WORD_ID *seq;
	int seqnum;
	int n, i;
	Sentence *s;
	RecogProcess *r;
	
	for (r = recog->process_list; r; r = r->next)
	{
		// �Z���e���X�������J��Ԃ��B
		winfo = r->lm->winfo;
		for (n = 0; n < r->result.sentnum; n++)
		{
			// �Z���e���X�\���̂̃A�h���X�擾�B
			s = &(r->result.sent[n]);
			// ���[�h�̏W�܂��ID���擾�B
			seq = s->word;
			// ���[�h�̐����擾�B
			seqnum = s->word_num;

			// ���[�h�������J��Ԃ��B
			for (i = 0; i < seqnum; i++)
			{								
				fprintf(stderr, "%s\n", winfo->woutput[seq[i]]);

				rapiro.writeBytes(winfo->woutput[seq[i]], 3);
			}
		}
	}
	// �o�b�t�@�N���A�B
	fflush(stdout);
}


int main(int argc, char *argv[])
{
	Jconf *jconf;	// �ݒ�p�����[�^�i�[�p�B
	Recog *recog;	// �F���R�A�̃C���X�^���X�B
	int ret;

	// �V���A���|�[�g��57600bps�ɐݒ�B
	rapiro.begin(57600);

	// -C�I�v�V�����Ŏw�肷��.jconf�t�@�C���̃`�F�b�N�B
	if (argc == 1)
	{
		fprintf(stderr, "Try, '-help' for more information.\n");
		rapiro.end();
		return -1;
	}

	// �w�肵��.jconf�t�@�C������ݒ��ǂݍ��ށB
	jconf = j_config_load_args_new(argc, argv);
	if (jconf == NULL)
	{
		fprintf(stderr, "Error in j_config_load_args_new\n");
		rapiro.end();
		return -1;
	}

	// �ǂݍ���jconf����F������쐬�B
	recog = j_create_instance_from_jconf(jconf);
	if (recog == NULL)
	{
		fprintf(stderr, "Error in j_create_instance_from_jconf\n");
		rapiro.end();
		return -1;
	}

	// �R�[���o�b�N�̐ݒ�B
	callback_add(recog, CALLBACK_EVENT_SPEECH_READY, callback_speech_ready, NULL);	// �F�����������B
	callback_add(recog, CALLBACK_EVENT_RECOGNITION_BEGIN, callback_recognition_begin, NULL);	// �F�������J�n�B
	callback_add(recog, CALLBACK_RESULT, callback_output_result, NULL);	// �F�����ʏo�́B

	// �}�C�N�ȂǃI�[�f�B�I����̏������B
	if (j_adin_init(recog) == FALSE)
	{
		// Error.
		fprintf(stderr, "Error in j_adin_init\n");
		rapiro.end();
		return -1;
	}

	// ���O�o�͗L���B
	j_recog_info(recog);

	// ���̓f�o�C�X���I�[�v������Ă��邩�m�F�B
	switch (j_open_stream(recog, NULL))
	{
	case 0:		// ����B
		break;
	case -1:	// �G���[�B
		fprintf(stderr, "error in input stream\n");
		return 0;
	case -2:
		fprintf(stderr, "failed to begininput stream\n");
		return 0;
	}

	// �F�����s�����[�v�ɓ���B
	ret = j_recognize_stream(recog);
	// ���������ŃG���[���N���������ǂ����B
	if (ret == -1)	return -1;

	// �I�������B
	// �X�g���[�������B
	j_close_stream(recog);
	// �C���X�^���X������B
	j_recog_free(recog);

	// �V���A���|�[�g�I���B
	rapiro.end();

	return 0;
}